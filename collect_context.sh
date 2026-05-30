#!/usr/bin/env bash
set -euo pipefail

# ============================================================
# Context collector (Git-independent)
# ============================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
OUTPUT_FILE="$PROJECT_ROOT/context.txt"
CONTEXT_IGNORE_FILE="$PROJECT_ROOT/.contextignore"

# ------------------------------------------------------------
# Default ignore patterns (always applied)
# ------------------------------------------------------------
DEFAULT_IGNORES=(
  ".git"
  ".gitignore"
  ".contextignore"
  "scripts/collect_context.sh"
  "context.txt"
  ".idea"
  ".vscode"
  "__pycache__"
  ".ruff_cache"
  "*.pyc"
  "*.pyo"
  "*.swp"
  ".DS_Store"
  ".venv"
  "venv"
  "env"
  "dist"
  "build"
  "*.egg-info"
)

# ------------------------------------------------------------
# Read .contextignore if present
# ------------------------------------------------------------
CONTEXT_IGNORES=()
if [[ -f "$CONTEXT_IGNORE_FILE" ]]; then
  while IFS= read -r line; do
    [[ -z "$line" || "$line" =~ ^# ]] && continue
    CONTEXT_IGNORES+=("$line")
  done < "$CONTEXT_IGNORE_FILE"
fi

# ------------------------------------------------------------
# Check if path should be ignored
# ------------------------------------------------------------
should_ignore() {
  local path="$1"

  for pattern in "${DEFAULT_IGNORES[@]}"; do
    [[ "$path" == $pattern || "$path" == */$pattern || "$path" == $pattern/* ]] && return 0
  done

  for pattern in "${CONTEXT_IGNORES[@]}"; do
    [[ "$path" == $pattern || "$path" == */$pattern || "$path" == $pattern/* ]] && return 0
  done

  return 1
}

# ------------------------------------------------------------
# Collect files
# ------------------------------------------------------------
mapfile -t ALL_FILES < <(
  cd "$PROJECT_ROOT"
  find . -type f \
    ! -path "./.git/*" \
    | sed 's|^\./||'
)

FILTERED_FILES=()
for file in "${ALL_FILES[@]}"; do
  should_ignore "$file" && continue
  FILTERED_FILES+=("$file")
done

# ------------------------------------------------------------
# Sorting priority
# ------------------------------------------------------------
sort_key() {
  case "$1" in
    README* ) echo "1:$1" ;;
    config/* ) echo "2:$1" ;;
    src/* ) echo "3:$1" ;;
    tests/* ) echo "4:$1" ;;
    * ) echo "5:$1" ;;
  esac
}

mapfile -t SORTED_FILES < <(
  for f in "${FILTERED_FILES[@]}"; do
    echo "$(sort_key "$f")"
  done | sort | cut -d: -f2-
)

# ------------------------------------------------------------
# Generate tree-like structure
# ------------------------------------------------------------
generate_tree_structure() {
  echo "PROJECT STRUCTURE"
  echo "================="
  echo

  # Collect all directories and files
  declare -A dirs
  declare -A root_files

  for file in "${SORTED_FILES[@]}"; do
    IFS='/' read -ra parts <<< "$file"

    if [[ ${#parts[@]} -eq 1 ]]; then
      # Root file
      root_files["${parts[0]}"]=1
    else
      # Build directory path
      local dir_path=""
      for ((i=0; i<${#parts[@]}-1; i++)); do
        if [[ $i -eq 0 ]]; then
          dir_path="${parts[i]}"
        else
          dir_path="${dir_path}/${parts[i]}"
        fi
        dirs["$dir_path"]=1
      done
    fi
  done

  # Sort everything
  mapfile -t sorted_dirs < <(
    for dir in "${!dirs[@]}"; do
      echo "$dir"
    done | sort
  )

  mapfile -t sorted_root_files < <(
    for file in "${!root_files[@]}"; do
      echo "$file"
    done | sort
  )

  # Print root files first
  local root_count=${#sorted_root_files[@]}
  for ((i=0; i<root_count; i++)); do
    local file="${sorted_root_files[$i]}"
    if [[ $i -eq $((root_count - 1)) && ${#sorted_dirs[@]} -eq 0 ]]; then
      echo "└── $file"
    else
      echo "├── $file"
    fi
  done

  # Print directories and their files
  local dir_count=${#sorted_dirs[@]}
  for ((d=0; d<dir_count; d++)); do
    local dir="${sorted_dirs[$d]}"

    # Check if this is the last item
    if [[ $d -eq $((dir_count - 1)) ]]; then
      echo "└── $dir/"
    else
      echo "├── $dir/"
    fi

    # Find files in this directory
    local dir_files=()
    for file in "${SORTED_FILES[@]}"; do
      if [[ "$file" == "$dir"/* ]]; then
        local basename="${file#$dir/}"
        # Direct child only (no further slashes)
        if [[ "$basename" != */* ]]; then
          dir_files+=("$basename")
        fi
      fi
    done

    # Sort and print files in this directory
    mapfile -t sorted_dir_files < <(printf '%s\n' "${dir_files[@]}" | sort)
    local file_count=${#sorted_dir_files[@]}

    for ((f=0; f<file_count; f++)); do
      local file="${sorted_dir_files[$f]}"

      # Determine prefix based on position
      if [[ $d -eq $((dir_count - 1)) ]]; then
        local prefix="    "
      else
        local prefix="│   "
      fi

      if [[ $f -eq $((file_count - 1)) ]]; then
        echo "${prefix}└── $file"
      else
        echo "${prefix}├── $file"
      fi
    done
  done

  echo
  echo "TOTAL FILES: ${#SORTED_FILES[@]}"
  echo "========================================"
  echo
}

# ------------------------------------------------------------
# Write output
# ------------------------------------------------------------
: > "$OUTPUT_FILE"

# Add file structure at the beginning
{
  echo "# ============================================================"
  echo "# PROJECT CONTEXT"
  echo "# Generated: $(date)"
  echo "# ============================================================"
  echo
  generate_tree_structure
  echo "# ============================================================"
  echo "# FILE CONTENTS"
  echo "# ============================================================"
  echo
} >> "$OUTPUT_FILE"

for file in "${SORTED_FILES[@]}"; do
  {
    echo "# ============================================================"
    echo "# path: $file"
    echo "# ============================================================"
    echo
    cat "$PROJECT_ROOT/$file"
    echo
    echo
  } >> "$OUTPUT_FILE"
done

# ------------------------------------------------------------
# Summary and clipboard
# ------------------------------------------------------------
echo "✅ Context file created at:"
echo "   $OUTPUT_FILE"

# Copy to clipboard
if command -v wl-copy &>/dev/null; then
    wl-copy < "$OUTPUT_FILE"
    echo "📋 Copied to clipboard (wl-copy)"
elif command -v xclip &>/dev/null; then
    xclip -selection clipboard < "$OUTPUT_FILE"
    echo "📋 Copied to clipboard (xclip)"
else
    echo "⚠ Install wl-clipboard or xclip for clipboard support"
fi
