# Conventional Commits Cheat Sheet

Try to follow the Conventional Commits specification to ensure
clear history, easier reviews, and automated changelog and release generation.

Based on: https://www.conventionalcommits.org/

## Quick Reference

```
<type>(<scope>): <description>

[optional body]

[optional footer(s)]
```

## Commit Format

```
<type>(optional scope): short, imperative description
```

**Examples:**
- `feat(auth): add login support`
- `fix(mqtt): handle reconnect correctly`
- `docs: update README installation steps`
- `chore(deps): update Qt to 6.7.0`

## Common Commit Types

### feat
Introduces a new feature or user-visible functionality.

**Example:**
```
feat(update): add progress reporting to updater
```

### fix
Bug fixes or corrections to existing behavior.

**Example:**
```
fix(mqtt): prevent retained state overwrite
```

### docs
Documentation only changes.
Includes README, inline docs, comments, or guides.

**Example:**
```
docs: clarify update entity behavior
```

### style
Code style or formatting changes with no behavior impact.
Whitespace, indentation, formatting, missing semicolons.

**Example:**
```
style: format source files with clang-format
```

### refactor
Code restructuring that does not change external behavior.
No bug fixes, no new features.

**Example:**
```
refactor(core): simplify update state handling
```

### test
Adding, updating, or fixing tests.

**Example:**
```
test(update): add downgrade scenario coverage
```

### chore
Maintenance tasks that do not affect runtime behavior.
Dependency updates, tooling, repo housekeeping.

**Example:**
```
chore: bump Qt minimum version
```

### build
Changes affecting the build system or dependencies.

**Example:**
```
build: adjust CMake install paths
```

### ci
Changes to CI configuration or workflows.

**Example:**
```
ci: add flatpak build job
```

### perf
Performance improvements without functional changes.

**Example:**
```
perf(mqtt): reduce state publish frequency
```

### revert
Reverts a previous commit.
Should reference the commit being reverted.

**Example:**
```
revert: revert "feat(update): add auto-install"
```

## Scope

An optional scope may be added to clarify what area is affected.

**Common scopes for this project:**
- `core` - Core framework functionality
- `mqtt` - MQTT integration and communication
- `bluetooth` - Bluetooth device integration
- `update` - Update/OTA functionality
- `auth` - Authentication and security
- `ui` - User interface components
- `dbus` - DBus integration
- `test` - Test infrastructure

**Examples:**
- `fix(bluetooth): handle device disconnection`
- `feat(update): add rollback support`
- `refactor(core): extract configuration manager`

## Breaking Changes

Breaking changes must be explicitly marked. These trigger major version bumps.

### Two valid ways to mark breaking changes:

1. **Add `!` after the type/scope:**
   ```
   feat!(api): change update payload format
   ```

2. **Add a footer in the commit body:**
   ```
   feat(api): update response format
   
   BREAKING CHANGE: update_percentage field is now required
   ```

### Complete breaking change example:
```
feat!(core): change entity ID format

BREAKING CHANGE: Entity IDs now use kebab-case instead of snake_case.
All existing automations using entity IDs need to be updated.
```

## Commit Body Guidelines

For complex changes, add a body after the subject line:

```
feat(update): add verification step before installation

- Add SHA256 checksum verification for downloaded updates
- Implement rollback mechanism on verification failure
- Update UI to show verification progress

Closes #123
```

**Body best practices:**
- Use bullet points for multiple changes
- Explain the "why" not just the "what"
- Reference issues with `Closes #123` or `Fixes #456`
- Keep lines under 72 characters

## Footer Section

Optional footers for additional metadata:

```
feat: add dark mode support

Reviewed-by: John Doe <john@example.com>
Signed-off-by: Jane Smith <jane@example.com>
Closes: #789
```

Common footer tags:
- `Reviewed-by:` - Code review approval
- `Signed-off-by:` - Developer signature (for DCO)
- `Closes:` - Issue tracking reference
- `See-also:` - Related commits or documentation

## Practical Guidelines

### Do:
- ✅ Use present tense, imperative mood
  - Good: "add support for X"
  - Bad: "added support for X"
- ✅ Keep the subject line concise (under 72 characters)
- ✅ One logical change per commit
- ✅ Use scoped commits in larger projects
- ✅ Write meaningful commit bodies for complex changes
- ✅ Reference related issues

### Don't:
- ❌ Write vague messages like "fix bug" or "update"
- ❌ Include unnecessary details in the subject line
- ❌ Mix unrelated changes in one commit
- ❌ Use past tense ("fixed", "added", "changed")

## Examples from Real Work

### Good examples:
```
fix(mqtt): handle QoS 2 message acknowledgment
```
```
feat(bluetooth): add battery level reporting for devices
```
```
docs: add DBus interface creation guide
```
```
refactor(core): extract MQTT client to separate class

- Improves testability of MQTT functionality
- Reduces coupling between core and network layers
- Maintains backward compatibility

Closes #234
```

### Bad examples:
```
fixed some stuff  # Too vague
```
```
update  # What was updated?
```
```
WIP: adding new feature  # Don't commit WIP
```

## Tooling Support

### Commitizen (Interactive commit helper):
```bash
npm install -g commitizen
cz init
```

### Commitlint (Validation):
```bash
# .commitlintrc.json
{
  "extends": ["@commitlint/config-conventional"]
}
```

### Git hooks with Husky:
```bash
# .husky/commit-msg
npx --no -- commitlint --edit "$1"
```

### Automatic changelog generation:
```bash
npx conventional-changelog -p angular -i CHANGELOG.md -s
```

## Why This Matters

Using Conventional Commits enables:

### For Developers:
- **Clear review history** - Understand changes at a glance
- **Structured commits** - Consistent format across the team
- **Easier debugging** - Trace changes to specific features/fixes

### For the Project:
- **Automatic changelog generation** - No manual release notes
- **Structured GitHub releases** - Automated version tagging
- **Predictable versioning** - Semantic versioning based on commit types
- **Better collaboration** - Clear communication of changes

### For Users:
- **Transparent development** - Understand what changed in each release
- **Easier upgrading** - Breaking changes are clearly marked
- **Better documentation** - Commit history serves as project documentation

## Common Questions

### Q: What if my commit doesn't fit any type?
A: Use `chore` for general maintenance or `refactor` for code changes without functional impact.

### Q: Should I squash commits?
A: Yes, squash feature branch commits before merging to maintain a clean history. Each commit should be a complete, working change.

### Q: What about merge commits?
A: Use conventional commit format for merge commits too: `Merge branch 'feature/x'` or `Merge pull request #123`.

### Q: How detailed should the body be?
A: Detailed enough that someone reading it 6 months later understands the context and reasoning.

---

**Remember:** Good commit messages are a gift to your future self and your team!