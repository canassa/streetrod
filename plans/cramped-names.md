# Budget-cramped names still constrained

Rechecked on naming5. Meanings remain established; the intended spellings below failed the hash window or the cumulative per-file compiler budget. Each budget trial compiled all changed code files; rejected trials were restored.

| Original placeholder | Current name | Intended name | Constraint |
|---|---|---|---|
| bucount | g_nScore | g_hallCount | Communal hash window |
| g7f32s | g_strOpp | g_rivalBanner | Communal hash window |
| cz8bd0y | g_edgeXR | g_roadRight | Communal hash window |
| F_1934E | ar_bm | ar_bitmap | GARAGE.C compiler budget |
| g_121c | g_gar | g_garageHots | GARAGE.C compiler budget |
| g_239e | g_txt | g_uiText | GARAGE.C compiler budget |
| g_39c9 | g_junk | g_junkWarning | GARAGE.C compiler budget |
| g_4733 | g_cVis | g_cursorDepth | GARAGE.C compiler budget |
| g_4f46 | g_old | g_oldCar | GARAGE.C compiler budget |
| g_4f48 | g_new | g_newCar | GARAGE.C compiler budget |
| g_5048 | g_colX | g_calendarX | GARAGE.C compiler budget |
| g_504a | g_rowY | g_calendarY | GARAGE.C compiler budget |
| g_5052 | g_wOff | g_calendarRows | GARAGE.C compiler budget |
| g_5082 | g_mon | g_months | GARAGE.C compiler budget |
| g_508a | g_hIds | g_carHotIds | GARAGE.C compiler budget |
| g_50c2 | g_paint | g_paintColors | GARAGE.C compiler budget |
| g_50ce | g_pad | g_infoIndents | GARAGE.C compiler budget |
| g_511e | g_iTxt | g_infoText | GARAGE.C compiler budget |
| g_5126 | g_nCarb | g_carbCounts | GARAGE.C compiler budget |
| g_5130 | g_nBolt | g_boltCounts | GARAGE.C compiler budget |
| g_5148 | g_cPos | g_carbBoltPositions | GARAGE.C compiler budget |
| g_5186 | g_bPos | g_boltPositions | GARAGE.C compiler budget |
| g_519a | g_bIdx | g_boltSides | GARAGE.C compiler budget |
| g_1804 | g_1804 | g_transBoltHots | Per-file compiler budget (naming5) |
| g_1e34 | g_1e34 | g_bodyModHots | Per-file compiler budget (naming5) |
| g_29ff | g_29ff | g_garageText | Per-file compiler budget (naming5) |
| g_49d2 | g_49d2 | g_transPics | Per-file compiler budget (naming5) |
| g_49dc | g_49dc | g_bankrollLabel | Per-file compiler budget (naming5) |
| g_5184 | g_5184 | g_boltSlots | Per-file compiler budget (naming5) |
| g_5198 | g_5198 | g_boltSubslots | Per-file compiler budget (naming5) |

Fixed names have been dropped. Five former entries are struct members: `g_videoOps.show`, `g_penPoint.x`, and `g_gameClock.actionTicks`/`biosTicks`/`timeBase`. Public function spellings retain the approved module prefixes. Evidence, rejected budget/hash trials, grouping candidates, and the applied ledger are under uncommitted `work/naming5/`.
