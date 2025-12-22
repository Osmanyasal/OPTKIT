# Changelog

All notable changes to OPTKIT will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/).

## [Unreleased]

### Added
- Error code system for distinguishing failure types
- RAII file descriptor wrapper for resource safety
- Unit tests for metric calculations
- GitHub Actions CI/CD pipeline
- Comprehensive troubleshooting guide

### Fixed
- Resource leaks on profiler construction failures
- Uninitialized variable usage in temperature profilers
- Integer overflow in slot calculations

### Changed
- Error messages now include actionable remediation steps

## [0.5.0] - 2025-01-12

### Added
- Initial multi-architecture support (Intel/AMD/ARM)
- Metric first profiling approach
- RAPL energy profiling
- GPU monitoring (NVIDIA/AMD)
- optkit-cli tool
- optkit-setenv tool
- Example benchmarks