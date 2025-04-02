# Commit Formatting Rules
Each commit should be formatted according to the 'commit format' exposed in this document. It allows us to quickly find and understand commits and changes.

### Subject
The subject must be written as follows:
`<action type>([scope]): <description>`

- The commit subject must be under 80 characters long.
- If the commit subject is too long, then you may have made too many unrelated changes for a single commit.
- The commit subject must also contain only lowercase letters.
- It must be written using the imperative voice and in the present tense
- The subject must not end with a '.'.
- The scope is optionnal. It allows us to tell which area of the project a single commit affects. If the commit changes only a specific part of the project, feel free to specify the scope of such commit.

Action types:
- feat --> Addition/implementation of a new feature
- fix --> Bug fixing commit
- task -->
- docs --> Addition/modification of the documentation
- ci/cd --> Anything related to CI and CD (GitHub Actions)
- refactor --> Reorganize code without changing the functionnality of the program
- test --> Addition/modification of tests

Example of well formatted commit subjects:

- `feat: add xbox controller support`
- `fix(CPU): prevent overriding of registers` -> commit with defined scope
- etc.

### Body

The body of the commit message is optionnal but strongly recommended for large or breaking/major changes. It is important to describe all the changes that have been made (features added/removed, which bugs have been fixed etc) and the reason they have been made.


> [!NOTE]
> RogEm's 'commit format' is based on the 'Conventional Commits' document.
> Please take a little time to read the specification for more details
> _https://www.conventionalcommits.org/en/v1.0.0/_
