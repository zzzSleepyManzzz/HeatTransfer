#include "HeatTransfer/Solver/ISolver.h"

#include <algorithm>
#include <stdexcept>

#include <tracy/Tracy.hpp>

#include "HeatTransfer/Core/Tensor3dMapper.h"

#include "HeatTransfer/Solver/Discretisation/Geometry.h"
#include "HeatTransfer/Solver/Discretisation/Stencil.h"

namespace HeatTransfer::Solver
{
    ISolver::ISolver(const Core::SimulationParameters& parameters,
                     const Core::MaterialProperties& materialProperties,
                     const Core::BoundaryConditions& boundaryCondition,
                     const std::optional<Core::InitialConditions>& initialConditions)
        : _parameters(parameters)
        , _materialProperties(materialProperties)
        , _boundaryCondition(boundaryCondition)
        , _initialConditions(initialConditions)
    {
    }

    void ISolver::ComputeSteadyStateSimulation(std::stop_token stopToken)
    {
        ZoneScoped;

        const int rows = _parameters.Rows;
        const int columns = _parameters.Columns;
        const int layers = _parameters.Layers;

        _temperatureTensor = std::make_shared<Core::Tensor3d>(rows, columns, layers);
        _temperatureTensor->setZero();

        _residualTensor = std::make_shared<Core::Tensor3d>(rows, columns, layers);
        _residualTensor->setZero();

        const auto geometry =
            std::make_shared<Geometry>(_parameters, _materialProperties, _boundaryCondition);

        geometry->ApplyFixedTemperatures(*_temperatureTensor);

        _system = std::make_shared<SystemMatrix>(geometry->BuildFixedTemperatureMask(),
                                                 geometry->BuildBiotNumbers());
        _rhs = geometry->BuildSteadyStateRhs();

        Initialise();

        IterateTemperature(stopToken);
    }

    void ISolver::ComputeTransientSimulation(std::stop_token stopToken)
    {
        ZoneScoped;

        if (!_initialConditions.has_value())
            throw std::invalid_argument("Transient simulation needs initial conditions");

        const int rows = _parameters.Rows;
        const int columns = _parameters.Columns;
        const int layers = _parameters.Layers;

        _temperatureTensor = std::make_shared<Core::Tensor3d>(rows, columns, layers);
        _temperatureTensor->setConstant(_initialConditions.value().Temperature);

        _residualTensor = std::make_shared<Core::Tensor3d>(rows, columns, layers);
        _residualTensor->setZero();

        const auto geometry =
            std::make_shared<Geometry>(_parameters, _materialProperties, _boundaryCondition);

        geometry->ApplyFixedTemperatures(*_temperatureTensor);

        _system = std::make_shared<SystemMatrix>(geometry->BuildFixedTemperatureMask(),
                                                 geometry->BuildBiotNumbers(),
                                                 geometry->BuildInverseFourierNumber());

        const auto steadyStateRhs = geometry->BuildSteadyStateRhs();
        const auto heatCapacity = geometry->BuildHeatCapacity();

        for (auto timeStep = 1; timeStep <= _parameters.NumberOfTimeSteps; timeStep++)
        {
            if (stopToken.stop_requested())
                return;

            // b = steady-state rhs + heat capacity * previous temperature / 6
            _rhs = steadyStateRhs + heatCapacity * (*_temperatureTensor) / Stencil::SCALE;

            Initialise();

            IterateTemperature(stopToken);

            const auto T = Core::Tensor3dMapper::AsVector(*_temperatureTensor);

            auto timeStepMetric =
                std::make_shared<Core::TimeStepMetric>(timeStep,
                                                       timeStep * _parameters.TimeStep,
                                                       GetCurrentIteration(),
                                                       T.maxCoeff(),
                                                       T.mean(),
                                                       T.minCoeff());

            _timeStepMetrics.push_back(timeStepMetric);
        }
    }

    void ISolver::PrintResidual()
    {
        for (const auto& residual : _residualMetrics)
        {
            std::cout << std::format("Iteration     : {}", residual->Iteration) << std::endl;
            std::cout << std::format("Max residual  : {}", residual->ResidualMax) << std::endl;
            std::cout << std::format("Mean residual : {}", residual->ResidualMean) << std::endl;
            std::cout << std::format("RMS residual  : {}", residual->ResidualRMS) << std::endl;
            std::cout << std::endl;
        }
    }

    void ISolver::PrintTimeSteps()
    {
        for (const auto& timeStep : _timeStepMetrics)
        {
            std::cout << std::format("Step              : {}", timeStep->TimeStep) << std::endl;
            std::cout << std::format("Time              : {} s", timeStep->Time) << std::endl;
            std::cout << std::format("Num of Iterations : {}", timeStep->Iterations) << std::endl;
            std::cout << std::format("Min Temperature   : {} K", timeStep->TemperatureMin)
                      << std::endl;
            std::cout << std::format("Mean Temperature  : {} K", timeStep->TemperatureMean)
                      << std::endl;
            std::cout << std::format("Max Temperature   : {} K", timeStep->TemperatureMax)
                      << std::endl;
            std::cout << std::endl;
        }
    }

    bool ISolver::IsConverged()
    {
        using enum HeatTransfer::Core::TrackingMetric;

        auto finalResidual = _residualMetrics.back();

        double finalTargetResidual;

        switch (_parameters.ResidualTrackingMetric)
        {
            case MAX:
                finalTargetResidual = finalResidual->ResidualMax;
                break;
            case MEAN:
                finalTargetResidual = finalResidual->ResidualMean;
                break;
            case RMS:
                finalTargetResidual = finalResidual->ResidualRMS;
                break;
            default:
                throw std::runtime_error("Unknown residual tracking metric found");
        }

        return finalTargetResidual <= _parameters.ResidualTolerance;
    }

    int ISolver::GetCurrentIteration() const
    {
        return _currentIteration;
    }

    const std::vector<std::shared_ptr<Core::ErrorMetric>>& ISolver::GetErrorMetrics()
    {
        return _errorMetrics;
    }

    const std::vector<std::shared_ptr<Core::ResidualMetric>>& ISolver::GetResidualMetrics()
    {
        return _residualMetrics;
    }

    std::shared_ptr<Core::Tensor3d> ISolver::GetTemperatureTensor()
    {
        return _temperatureTensor;
    }

    std::shared_ptr<Core::Tensor3d> ISolver::GetResidualTensor()
    {
        return _residualTensor;
    }

    void ISolver::IterateTemperature(std::stop_token stopToken)
    {
        ZoneScoped;

        const int maxIterations = _parameters.MaxIterations;
        const double residualTolerance = _parameters.ResidualTolerance;
        const double errorTolerance = _parameters.ErrorTolerance;

        int iterations = 0;

        std::vector<double> residualTracker(3, residualTolerance + 1.0);
        std::vector<double> errorTracker(3, errorTolerance + 1.0);

        auto indexR = (size_t)_parameters.ResidualTrackingMetric;
        auto indexE = (size_t)_parameters.ErrorTrackingMetric;

        // Preallocate for performance
        Core::Tensor3d oldTemperature(_temperatureTensor->dimensions());
        Eigen::ArrayXd errorArray(_temperatureTensor->size());

        while (!stopToken.stop_requested() && iterations < maxIterations &&
               residualTracker[indexR] > residualTolerance && errorTracker[indexE] > errorTolerance)
        {
            oldTemperature = *_temperatureTensor;

            Iterate();

            UpdateResidualTensor();

            errorArray = Core::Tensor3dMapper::AsVector(*_temperatureTensor).array() -
                         Core::Tensor3dMapper::AsVector(oldTemperature).array();
            const double errorMax = errorArray.abs().maxCoeff();
            const double errorMean = errorArray.abs().mean();
            const double errorRMS = std::sqrt(errorArray.square().mean());

            const auto residualArray = Core::Tensor3dMapper::AsVector(*_residualTensor).array();
            const double residualMax = residualArray.abs().maxCoeff();
            const double residualMean = residualArray.abs().mean();
            const double residualRMS = std::sqrt(residualArray.square().mean());

            residualTracker = {residualMax, residualMean, residualRMS};
            errorTracker = {errorMax, errorMean, errorRMS};

            iterations++;
            _currentIteration = iterations;

            auto error =
                std::make_shared<Core::ErrorMetric>(iterations, errorMax, errorMean, errorRMS);

            _errorMetrics.push_back(error);

            auto residual = std::make_shared<Core::ResidualMetric>(
                iterations, residualMax, residualMean, residualRMS);

            _residualMetrics.push_back(residual);
        }
    }

    void ISolver::UpdateResidualTensor()
    {
        ZoneScoped;

        // Compute the absolute residual |b - H * x|

        auto residual = std::make_shared<Core::Tensor3d>(
            Stencil::ComputeResidual(*_temperatureTensor, _rhs, *_system));

        auto R = Core::Tensor3dMapper::AsVector(*residual);
        R = R.cwiseAbs();

        _residualTensor = residual;
    }
}
