---
name: fill-mr
description: Fill the description of an existing GitLab merge request for the current branch. Use when a repository merge request must be found, its template loaded, its description generated, and its body updated through glab.
metadata:
  short-description: Fill an existing GitLab merge request
---

# Fill an existing merge request

Use this skill only for an existing merge request. Do not create a merge
request or change its title, reviewers, assignees, labels, or milestones unless
the user explicitly asks.

## Select the repository

By default, use the current repository. If the user supplies a first argument,
interpret it as the project name to resolve in the current workspace rather
than using a machine-specific absolute path:

- If no argument is supplied, use the current repository.
- If an argument is supplied, it must match exactly one project in the current
  workspace or one sibling checkout.
- If the current repository's root basename matches the argument, use it.
- Otherwise look for a project with that name in the current workspace or as a
  sibling checkout.
- Confirm the selected root with `git -C <root> rev-parse --show-toplevel`.
- If the repository cannot be resolved unambiguously, ask the user for the
  repository directory and do not run commands against another checkout.

Run all `git` and `glab` commands with the selected repository root as the
working directory. Never assume that a particular username, home directory,
workspace layout, or local data path exists.

## Workflow

1. Resolve the current repository, or resolve the first argument as a project
   name in the current workspace.
2. Run `git branch --show-current` in the selected repository.
3. Find opened merge requests for that source branch on the configured GitLab
   host, preferably with:

   ```text
   glab mr list --source-branch "<branch>" --state opened
   ```

4. If there is no opened MR, stop and explain. If there are several, ask the
   user which MR to update.
5. Inspect the current MR description for issue references such as `Closes
   #...`, `Relates #...`, `Blocks #...`, or GitLab issue/work-item URLs. Fetch
   referenced issue context when available. If none is found, ask whether the
   user wants to provide an issue URL or ID; continue without it if they
   decline and state that clearly.
6. Load the corresponding issue and identify its `Acceptance Criteria`
   section. These criteria are the source of truth for the MR template; never
   invent new criteria or replace issue criteria with implementation details.
7. Load `.gitlab/merge_request_templates/default.md` from the selected root.
   If it is absent, stop and report the expected path.
8. Ask whether the user wants to provide the `Description` section. Otherwise
   infer it from the non-merge commits and the branch diff.
9. Populate the MR `Acceptance Criteria` section from the linked issue:
   - Mark a criterion complete only when the MR description or diff provides
     sufficient evidence that it is satisfied.
   - If a criterion cannot be assessed from the MR description or diff, copy
     it verbatim without claiming completion and add a note that the user must
     complete it later.
   - Do not omit, merge, weaken, or reword criteria in a way that changes their
     meaning.
10. Generate the complete MR body while preserving the template's headings and
   mandatory sections.
11. Show the user the MR IID, URL, and a brief summary of the generated content
   before updating the MR.
12. Update only the existing MR description with the available `glab mr
    update` equivalent, passing the generated markdown safely through a
    temporary file or the command's file-input option.

## Content rules

- Write all MR content in English, even when the conversation is in another
  language.
- Use inline backticks for identifiers, functions, constants, and filenames.
- Use fenced code blocks with a language identifier for multiline excerpts.
- For multiple commits, include one concise description subsection per commit.
- Fill `Results` when the change has observable behaviour, performance,
  screenshots, or another meaningful outcome; otherwise write a short `N/A`.
- Fill `Breaking change(s)` only when a public API is broken. State whether
  downstream updates are complete or give concise migration steps.
- Derive `Acceptance Criteria` exclusively from the linked issue. Report
  explicitly whether all criteria could be completed from the available
  evidence. If not, leave unsupported criteria unchanged and tell the user
  which criteria still require manual completion.
- Keep `Data`, `Decreased code coverage`, and `Related issue(s)` unchanged from
  the repository template unless the user explicitly requests otherwise.
- Do not invent test results, issue details, review state, or performance data.

## Evidence gathering

Useful commands, all run from the selected repository root, include:

```text
git log --oneline --no-merges <base>..HEAD
git diff --name-status <base>..HEAD
git show --stat --summary <commit>
```

If the target branch is unknown, use the merge-base with the repository's
default remote branch. Keep generated content concise and actionable.

## Failure handling

- If the first argument does not match exactly one project in the workspace,
  ask for clarification or use the current repository when no argument was
  supplied.
- Missing MR: stop without modifying anything.
- Missing or ambiguous repository root: ask for the smallest missing path.
- Missing GitLab authentication: suggest `glab auth status` for the configured
  GitLab host.
- Any failed update: report the command error and do not claim success.

Return the MR IID and URL, a summary of filled sections, whether all acceptance
criteria were completed, the criteria left for the user, sections intentionally
left unchanged, and validation or update failures.
