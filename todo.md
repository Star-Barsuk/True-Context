## P0 — Critical (Must Have)

### 1. Implement Proper `.contextignore` Matching

**Problem**
Current implementation does not support directory matching correctly.

Examples:

* `test_1` → does not work
* `test_1/` → does not work
* `test_1/*` → works

**Goal**
Implement GitIgnore-style pattern matching.

**Requirements**

* Support directory rules:

  * `test_1/`
  * `templates/`
* Support file patterns:

  * `*.log`
  * `*.tmp`
* Support recursive patterns:

  * `**/node_modules/`
  * `**/*.min.js`
* Support exact path matching.

**Expected Result**
Ignore rules behave exactly as users expect.

---

### 2. Add Negation Support

**Problem**
Cannot re-include files after exclusion.

**Examples**

```text
tests/*
!tests/golden/*
```

```text
src/*
!src/main.py
```

**Goal**
Support GitIgnore-style negation rules.

---

### 3. Skip Binary Files

**Problem**
Binary files can corrupt generated context.

Examples:

* png
* jpg
* pdf
* sqlite
* zip

**Goal**
Detect binary files before reading.

**Expected Output**

```text
[BINARY FILE]
```

or

```text
[BINARY FILE - SKIPPED]
```

---

### 4. Add File Size Limits

**Problem**
Large files can explode context size.

Examples:

* package-lock.json
* vendor.js
* generated OpenAPI schemas
* large logs

**Goal**
Introduce configurable maximum file size.

Example:

```bash
MAX_FILE_SIZE=500000
```

**Expected Output**

```text
[FILE TOO LARGE: 3.2 MB]
```

---

### 5. Fix Tree Generation

**Problem**
Current tree output is not a real tree.

Current:

```text
tests/
tests/test_1/
tests/test_2/
tests/test_2/test_3/
```

Desired:

```text
tests/
├── test_1/
│   ├── file1
│   └── file2
└── test_2/
    ├── file4
    └── test_3/
        └── file3
```

**Goal**
Generate an actual hierarchical tree structure.

---

## P1 — High Priority

### 6. Refactor Architecture

Current implementation is mostly monolithic.

**Target Structure**

```text
collect_files()
filter_files()
sort_files()
render_tree()
render_contents()
```

**Goal**
Improve maintainability and testing.

---

### 7. Optimize Ignore Processing

**Problem**
Current complexity:

```text
O(files × patterns)
```

Can become expensive on large repositories.

**Goal**
Move filtering closer to the `find` stage where possible.

Example:

```bash
find . \
  -not -path "*/node_modules/*" \
  -not -path "*/dist/*"
```

---

### 8. Add Context Statistics

Current:

```text
TOTAL FILES: 123
```

Desired:

```text
TOTAL FILES: 123
TOTAL LINES: 15234
TOTAL CHARS: 420000
ESTIMATED TOKENS: 105000
```

**Goal**
Provide visibility into prompt size before sending to an LLM.

---

### 9. Fix Self-Ignore Rules

**Problem**
Default ignore list contains:

```text
scripts/collect_context.sh
```

Actual file:

```text
collect_context.sh
```

**Goal**
Ensure collector never includes itself accidentally.

---

## P2 — Medium Priority

### 10. Add Partial Collection

Allow collecting only a subdirectory.

Examples:

```bash
./collect_context.sh src/
```

```bash
./collect_context.sh tests/
```

```bash
./collect_context.sh src/services/
```

**Goal**
Generate focused contexts.

---

### 11. Configurable Sorting Priorities

Current sorting is hardcoded:

```text
README
config
src
tests
```

**Problem**
Does not fit all projects.

**Goal**
Move priorities into configuration.

Example:

```text
README
docs
src
app
lib
packages
tests
```

---

### 12. Improve Output Metadata

Add information per file:

```text
Path: src/user_service.py
Size: 48 KB
Lines: 820
Language: Python
```

**Goal**
Improve context readability.

---

## P3 — Nice To Have

### 13. Summary Mode

Add:

```bash
--summary
```

Instead of full content:

```text
Path: user_service.py

Lines: 820

Functions:
- create_user()
- update_user()
- delete_user()

Classes:
- UserService
- UserRepository
```

**Goal**
Generate lightweight architecture-focused context.

---

### 14. Multiple Output Formats

Support:

```bash
--format txt
--format md
--format xml
--format json
```

**Goal**
Allow integration with different tooling and LLM workflows.

---

### 15. Dependency-Aware Sorting

Instead of generic sorting:

```text
README
package.json
requirements.txt
src
tests
```

**Goal**
Present files in the order most useful for LLM understanding.

---

### 16. Tree Renderer Simplification

Current tree generation logic is overly complex.

**Options**

* Use `tree` if available.
* Use a dedicated tree renderer.
* Build a directory graph first, then render.

**Goal**
Reduce code complexity and improve correctness.

---

## Future Ideas

### 17. Include / Exclude CLI Flags

Examples:

```bash
--include "*.py"
--include "*.js"

--exclude "*.log"
--exclude "*.tmp"
```

---

### 18. Token Budget Mode

Example:

```bash
--max-tokens 100000
```

Collector automatically stops when limit is reached.

---

### 19. Smart Context Mode

Prioritize files by importance:

```text
README
package.json
requirements.txt
main entrypoint
core modules
tests
```

Skip low-value files automatically.

---

### 20. LLM-Oriented Context Profiles

Examples:

```bash
--profile review
--profile bugfix
--profile architecture
--profile onboarding
```

Different profiles generate different context layouts optimized for specific tasks.
