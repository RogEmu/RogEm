# QA Strategy - RogEm

## Overview

The QA (Quality Assurance) strategy aims to guarantee code reliability, maintainability, and quality through a set of practices integrated into the development lifecycle. This approach relies on automation, collaboration, and agile methodology to deliver a performant and stable PlayStation emulator.

## Strategic Objectives

### Primary Objectives
- **Ensure code stability**: Prevent the introduction of regressions into the codebase
- **Facilitate maintenance**: Produce clean, tested, and documented code
- **Accelerate delivery**: Reduce time between development and production deployment
- **Improve collaboration**: Standardize practices across the team
- **Guarantee traceability**: Track project evolution and changes

### Quality Objectives
- **Test coverage**: Maximize unit test coverage on critical components
- **Bug reduction**: Detect and fix bugs before production deployment
- **Standards compliance**: Respect development and versioning best practices
- **Documentation**: Maintain up-to-date and accessible documentation

## Key Strategy Points

### 1. Continuous Integration (CI) with GitHub Actions

Test automation via GitHub Actions is the pillar of our QA strategy:

- **Automated tests**: Systematic execution of the test suite on every Pull Request targeting `dev`
- **Early validation**: Immediate detection of compilation or test failures
- **Fast feedback**: Developers are instantly notified of issues
- **PR blocking**: No PR can be merged if tests fail

### 2. Systematic Code Reviews

Every code modification goes through a rigorous review process:

- **Mandatory review**: All PRs must be approved by the PM before merge
- **Technical discussion**: Implementation details are debated and improved
- **Knowledge sharing**: Reviews enable the diffusion of best practices
- **Early detection**: Identification of design or logic issues before integration

### 3. Stratified Branch Architecture

The branch structure ensures stability and traceability:

- **`main` branch**: Tested and validated production code, ready for releases
- **`dev` branch**: Continuous integration of new features
- **Feature branches**: Work isolation per issue, facilitating parallel development
- **Branch protection**: Only validated PRs can be merged to `dev` and `main`

### 4. Semantic Versioning (SemVer)

Semantic versioning ensures clear communication about software evolution:

- **`major.minor.patch` format**: Readable and standardized version
- **Tracked releases**: Each release on `main` corresponds to a specific version
- **Backward compatibility**: Version changes communicate their impact
- **Clear history**: Facilitates rollback and maintenance

### 5. Mandatory Unit Tests

Unit tests are at the heart of our quality guarantee:

- **Required tests**: Every new feature must be accompanied by tests
- **Local validation**: Developers run tests before pushing code
- **Verified values**: Test assertions are based on validated expected behaviors
- **Non-regression**: Existing tests protect against future regressions

### 6. Agile Methodology and Sprints

The agile approach structures our development and quality assurance process:

- **3-week sprints**: Short cycles for continuous value delivery
- **Sprint reviews**: Regular evaluation of performance and delivered quality
- **Retrospectives**: Continuous improvement of the development process
- **Adaptation**: Adjustment of QA practices based on feedback

### 7. Kanban and Issue Tracking

GitHub Project and Kanban ensure transparency and organization:

- **Complete traceability**: Every modification is linked to a documented issue
- **Clear prioritization**: Tasks are sorted and assigned with estimation points
- **Structured labels**: Scope, sprint, and status clearly identified
- **WIP minimization**: Limiting work in progress to improve quality

### 8. Standardized Workflow

A unified process from code creation to integration:

1. Create an issue with scope, points, and status
2. Dedicated branch from `dev`
3. Development + unit tests
4. Local test validation
5. Commit with closing keywords
6. Pull Request with assigned reviewers
7. Automated CI tests
8. Code review and discussions
9. Merge to `dev` after validation

## Metrics and Indicators

To measure the effectiveness of our QA strategy, we track:

- **Test success rate**: Percentage of passing tests
- **PR cycle time**: From opening to merge
- **Production bug count**: Detected after release
- **Code coverage**: Percentage of code covered by tests
- **Sprint velocity**: Points completed per sprint

## Responsibilities

- **Developers**: Write tests, validate locally, respect the workflow
- **PM**: Review PRs, maintain standards, manage releases
- **Team**: Participate in reviews, communicate blockers, improve the process

## Continuous Improvement

The QA strategy is a living process that evolves with the project:

- **Regular sprint reviews**: Identification of improvement points
- **Open communication**: Any member can propose improvements
- **Practice adaptation**: Adjustment based on needs and feedback
- **Up-to-date documentation**: Process changes are documented

---

> [!NOTE]
> This QA strategy naturally integrates into our work methodology described in [PROJECT_GUIDELINES.md](PROJECT_GUIDELINES.md). It aims to create a development environment where quality is everyone's responsibility, at every stage of the process.
