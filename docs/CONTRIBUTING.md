Contributing to the Robots Project
=========================

## Code Style ##

For C++ code we will follow the [Google C++ style guide](https://google.github.io/styleguide/cppguide.html) with some modifications.
For code formatting we will use the [K & R indent style](https://en.wikipedia.org/wiki/Indent_style#K.26R_style) with tabs for indentation (not spaces as in the Google style guide).
We will allow use of default arguments (not allowed in Google Style Guide).
C-style casts are allowed for primitive types.
Exceptions can be used (not allowed in Google Style Guide).

Go doesn't have an official style guide as such, but a list of common mistakes can be found [here](https://github.com/golang/go/wiki/CodeReviewComments).
Also worth a look is [Effective Go](https://golang.org/doc/effective_go.html).

## Code Formatting for Go code ##

Go formatting must be done with `go fmt` before checking code into the repo to ensure correct style for Go code.
`go lint` can be used to check code for style errors. `go vet` can be used to check for code that breaks Go coding practice.

## Commenting ##

Comments should be used to clarify the function of any section of code which may not be obvious.
Comments will also be used for creating documentation for each class which will be generated with documentation generation tools.

Doxygen will be used to generate documentation for C++ code. Information on Doxygen docblocks can be found [here](http://www.stack.nl/~dimitri/doxygen/manual/docblocks.html).

For Go code commenting will be done in accordance with the standard Go style used for generating Godocs.
An explaination of standard Go commenting practice and Godoc can be found [here](https://blog.golang.org/godoc-documenting-go-code).

## Pull Requests ##
Code must not be checked in to the projects master branch directly, a pull request must be created from a different branch (see Branches section).
Code will be reviewed by at least one other team member and merged by someone other than the creator of the pull request.
A pull request must contain a single feature, several distinct features must not be in one pull request.

## Branches ##

Branches should be used for one pull request only and should be deleted after the pull request is merged.

Branches should have a short prefix describing what type of changes are contained in it.
These prefixes should be one of the following:

* **feature/** -- for changes which add/affect a feature.
* **doc/** -- for changes to the documentation.
* **hotfix/** -- for quick bugfixes.

## Waffle.io ##

Waffle.io should be used to manage scrum items. Items in the "Ready" column are ready for work to start, when work is started on an issue it should be moved to the "In Progress" column. When finished an item should be moved to the "Completed" column. At the sprint review, items will be recycled or moved to the "Done" column.

When a pull request for an issue is merged then it should be moved to the "Done" column.
