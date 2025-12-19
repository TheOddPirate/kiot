# Conventional Commits Cheat Sheet

Try to follow the Conventional Commits specification to ensure
clear history, easier reviews, and automated changelog and release generation.

## Commit format

<type>(optional scope): short, imperative description

Examples:
- feat(auth): add login support
- fix(mqtt): handle reconnect correctly
- docs: update README installation steps

## Common commit types

### feat
Introduces a new feature or user-visible functionality.

Example:
feat(update): add progress reporting to updater

### fix
Bug fixes or corrections to existing behavior.

Example:
fix(mqtt): prevent retained state overwrite

### docs
Documentation only changes.
Includes README, inline docs, comments, or guides.

Example:
docs: clarify update entity behavior

### style
Code style or formatting changes with no behavior impact.
Whitespace, indentation, formatting, missing semicolons.

Example:
style: format source files with clang-format

### refactor
Code restructuring that does not change external behavior.
No bug fixes, no new features.

Example:
refactor(core): simplify update state handling

### test
Adding, updating, or fixing tests.

Example:
test(update): add downgrade scenario coverage

### chore
Maintenance tasks that do not affect runtime behavior.
Dependency updates, tooling, repo housekeeping.

Example:
chore: bump Qt minimum version

### build
Changes affecting the build system or dependencies.

Example:
build: adjust CMake install paths

### ci
Changes to CI configuration or workflows.

Example:
ci: add flatpak build job

### perf
Performance improvements without functional changes.

Example:
perf(mqtt): reduce state publish frequency

### revert
Reverts a previous commit.
Should reference the commit being reverted.

Example:
revert: revert "feat(update): add auto-install"

## Scope

An optional scope may be added to clarify what area is affected.

Examples:
- fix(bluetooth):
- feat(update):
- refactor(core):

## Breaking changes

Breaking changes must be explicitly marked.

Two valid ways:

1. Add ! after the type or scope
feat!(api): change update payload format

2. Add a footer in the commit body
BREAKING CHANGE: update_percentage is now required

Breaking changes are used for major version bumps.

## Guidelines

- Use present tense, imperative mood
  Good: "add support for X"
  Bad: "added support for X"

- Keep the subject line concise
  Ideally under 72 characters

- One logical change per commit

- Prefer scoped commits in larger projects

## Why this matters

Using Conventional Commits enables:
- Automatic changelog generation
- Structured GitHub releases
- Clear review history
- Predictable versioning
