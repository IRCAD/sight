---
name: create-issue
description: "Create a structured GitLab issue from the selected workspace project's issue template."
argument-hint: "[project-name]"
---

# Create a GitLab issue from a template

Use the repository skill at `.agents/skills/create-issue/SKILL.md` for
the complete workflow. With no argument, use the current repository. With a
first argument, resolve that project name against the current workspace and
require an unambiguous match.

Inspect the selected project's `.gitlab/issue_templates/` directory, ask for
the issue type and description, and show the proposed title, labels, assignees,
project, and complete body before creating anything. Keep all issue content in
English and update GitLab only after explicit user confirmation.
