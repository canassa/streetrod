---
name: wrap-up
description: End-of-session routine for the Street Rod project. Re-runs the regression gate, regenerates docs/PROGRESS.md, appends the captain's log entry to plans/diary.md, and folds what the session learned back into the skills and CLAUDE.md. Use at the end of any session with real work, or when the user says "wrap up", "log this", or "update the skills".
---

# wrap-up

## 1. Gate and progress

```
./sr check --all          # must be green; if not, fix or report the regression before anything else
./sr status               # regenerates docs/PROGRESS.md
```

## 2. Diary

Append to `plans/diary.md` (never edit earlier entries):

```
## <`date '+%Y-%m-%d %H:%M %Z'`>: <short title>

### What I did
- functions matched (addr, name), tools changed, decisions made

### What I learned
- codegen tells, wrong assumptions corrected, open questions
```

Link to the skill or file where a lesson now lives instead of repeating it.

## 3. Feed lessons back

Go through "What I learned" and put each lesson where the next session will see it:

| Lesson | Goes to |
|---|---|
| an instruction pattern ↔ C shape, or a divergence and its fix | `msc51-recipe-book` (one table row) |
| a step in the loop that was missing, wrong or in the wrong order | `decompile-function` |
| a fact about the binary, a new rule, a changed default (flags, layout) | `CLAUDE.md` |
| a CLI bug, gotcha or new command | fix it with `sr-cli`; document in `CLAUDE.md` |
| a file boundary (LAYOUT.JSON unit) or a file's flags | `./sr note` on the functions, plus the diary |

Keep skills short. When you add a row, check whether an older row is now redundant or wrong, and fix it.

## 4. Report

Tell the user what was matched, what is blocked and why, and anything that changes the plan.
