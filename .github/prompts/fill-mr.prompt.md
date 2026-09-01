---
name: fill-mr
description: "Fill the description of an existing GitLab merge request for the current branch in a selected workspace project."
argument-hint: "[project-name]"
---

# Fill an existing merge request

Use the repository skill at `.agents/skills/fill-mr/SKILL.md` for the complete
workflow. With no argument, use the current repository. With a first
argument, resolve that project name against the current workspace and require
an unambiguous match.

Resolve the selected checkout from the current workspace and verify it with
`git rev-parse --show-toplevel`; do not use hard-coded home directories,
usernames, workspace paths, or local data paths. Run all `git` and `glab`
commands from the selected repository root.

Before changing anything, identify the existing opened MR, load its default
template, and show the user the MR IID, URL, and generated section summary.
Update only the existing MR description. Keep mandatory template sections and
report exactly which validation checks ran.
