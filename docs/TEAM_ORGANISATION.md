# Team's Organisation

This is a simple guide to help you get started working on the RogEm project.

## Project Context

### Branches

There are 2 main branches on this repository:

***main*** branch: Used for the "released" versions. It contains production code which has been tested. This branch also contains the features implemented in version `major.minor.patch`

***dev*** branch: Used to continuously integrate new features or bug fixes required by open issues. New code is automatically tested when opening a PR or merging/rebasing onto `dev` branch.

Feature branches should be created when working on an issue (feature/bug fix etc.). After finishing work on an issue, a PR (Pull Request) to merge on branch `dev` should be opened, reviewed and then merged if accepted.

### GitHub Project / Kanban

The GitHub project allows us to track the issues that are currently worked on, waiting to be picked up, or that are being reviewed for example. It allows us to efficiently distribute the workload between team members, and accurate workload prediction.

For each issue that is defined on the project, we mark it with several speficic tags:

- *Points* -> Task difficulty/complexity/amount of time, on a modified [Fibonacci scale](https://www.lucidchart.com/blog/fibonacci-scale-for-agile-estimation)
- *Sprint* -> Time frame within which the issue is worked on
- *Labels* -> Tags which define the scope/subject of the issue
- *Status* -> Current work status of the issue

The [Kanban](https://kissflow.com/project/agile/kanban-methodology/) is the tool that let's us to quickly visualize the current state of a sprint and minimize WIP tasks. Please read the linked page if you are not familiar with the **Agile** methodology or **Kanban** tool.

### Work methodology and organization

Recurrent team meetings should take place to ensure efficiency and transparent communication. If **any** team member has **any** difficulty, it can be brought up dureing team meetings as well (at any time really, communication is key for this project).

#### The Sprint

During a sprint, we define a specific set of tasks (*issues*) that have to be completed by the end of such sprint. We have defined a sprint t be 3 weeks long. At the end of each sprint, a sprint review should take place to discuss what went right and wrong during the sprint. It allows the team to check their performances, better plan the next sprint and evaluate task difficulty/complexity/amount of time more accurately.

New issues can be added to the current sprint, if they have a logical relationship with a current task that is being worked on.

### Versioning

To coherently and properly version our codebase and releases, please refer to [semantic versioning](https://semver.org/).


## Workflow

### Some basic guidelines

First of all **make sure you are up to date with the remote**. Don't forget to pull the latest changes that have been made. This avoids merging in panic because you ahven't pulled the branch in a long time... When pulling, **use the rebase pulling method** to avoid unnecessary merge commits along the way. You can use the `git pull --rebase` to directly update the branch or set the defatul pull behavior of git as follows:

```
$ git config --global pull.rebase true
```

Using `git config` is the preferred way.


### Begin work

1. **Open an issue**
   - Define the purpose of the issue, its scope (labels), task points[^Point_estimation] and current status (generally ready or backlog)
2. **Link this issue to a newly created branch**
   - Remember to correclty choose the parent branch of the new feature branch, by default it should be `dev`
3. **Start working on the issue on its corresponding feature branch**
4. **Write unit tests (unless it is impossible or cumbersome)**
   - When writing unit tests, please make sure the expected test values and behaviors are correct and verified. A test with unverified expected values or behaviors is useless and obfuscated real problems
5. **Run the tests locally to ensure tests are passing**
6. **Commit and push the code onto the remote branch of your local branch**
   - When commiting, don't forget the **closing keywords** to traxk the issue related to this commit
   - i.e for *78-my-feature-branch* the remote is *origin/78-my-feature-branch*
7. **Open a Pull Request on GitHub**
   - Select the parent branch to megre your branch onto
   - Default is to merge on `dev` branch
   - Set the PM as a reviewer on the PR
8. **Set the issues that are tacked by the PR**
   - This setting is situated under the **developpement** menu on the right hand side of the PR
9. **Tests are run automatically on every pull request targeting `dev`**
   - If all tests pass, then the PR can be reviewed
   - After review there may be discussions about implementation details.
10. **Finally, the PR shall be merged onto `dev`**


> [!NOTE]
> If you have any question or suggestion please do not hesitate to reach out !


[^Point_estimation]: To accurately estimate points for a task, please have a look at this document [document](https://asana.com/resources/story-points) first
