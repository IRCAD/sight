---
name: create-issue
description: Create a structured GitLab issue from the selected project's issue templates. Use when an issue must be created for the current workspace project or another unambiguously resolved project.
metadata:
  short-description: Create a templated GitLab issue
---

# Create a GitLab issue from templates

Create issues only after collecting the required information and showing the
user the proposed title, project, labels, and description. Use the configured
GitLab instance through `glab`. Do not modify repository files or create an
issue until the user confirms the final proposal.

## Select the project

With no argument, use the current repository. If a first argument is supplied,
interpret it as a project name and resolve it against the current workspace or
its sibling checkouts. The name must match exactly one project.

Confirm the selected root with `git -C <root> rev-parse --show-toplevel` and
run repository commands from that root. Never hard-code a home directory,
username, workspace layout, or private project path. If resolution is missing
or ambiguous, ask the user for the smallest missing information.

The issue's GitLab project defaults to the selected repository. A different
GitLab project may be used only when the user explicitly supplies or confirms
it.

## Interactive workflow

1. Ask for the issue type: `fix`, `feat`, `enh`, or `refactor`.
2. Ask for a concise issue description. Require enough information to explain
   the problem or desired outcome; do not invent missing requirements.
3. Ask optionally for GitLab assignees. An empty answer means no assignee.
4. Load the matching template from
   `.gitlab/issue_templates/` in the selected repository:

   | Type | Template |
   | --- | --- |
   | `fix` | `Bug.md` |
   | `feat` | `Feature.md` |
   | `enh` | `Enhancement.md` |
   | `refactor` | `Refactor.md` |

   Stop and report the expected path if the template is missing.
5. Preserve the template's structure and populate its description and relevant
   sections. Keep mandatory project-specific text intact.
6. Generate a concise English title, at most 80 characters, using the selected
   type as a prefix (`fix:`, `feat:`, `enh:`, or `refactor:`). Start the subject
   with a lowercase letter.
7. Derive labels from the template or existing project convention. Do not
   claim a label exists without checking when that matters to the command.
8. Show the complete proposed title, project, labels, assignees, and body.
9. After confirmation, create the issue with `glab issue create` and return its
   URL.

## Markdown and command safety

- Write all issue content in English, even when the conversation is in another
  language.
- Use inline backticks for identifiers, functions, constants, and filenames.
- Use fenced code blocks with a language identifier for multiline excerpts.
- Build the description in a temporary file or use the CLI's file-input
  mechanism so backticks, lists, links, and special characters are preserved.
- Do not expose temporary files in the repository or commit them.
- Do not invent issue links, assignees, labels, test results, or project data.

## Failure handling

- Invalid issue type: ask again using the four supported types.
- Empty or insufficient description: ask for clarification.
- Missing `glab`: explain that the CLI is required.
- Authentication failure: suggest `glab auth status` for the configured GitLab
  host.
- Project resolution failure: stop before any remote mutation.
- Creation failure: report the command error and do not claim success.

Return the created issue URL, title, project, labels, and any information that
was intentionally left unspecified.
