# Fastest opening car in Street Rod

**Buy the $475 1949 Chevrolet Styleline. Normal starting cash is $750, not $400.** Before earning race winnings or farming donor cars, the best acceleration build is its **327 V8 + GM three-speed + single four-barrel intake + stripped bumpers** ($750 net). For the highest speed rating, keep its 283 V8 and use **GM three-speed + Dual-Quad intake + chopped roof + stripped bumpers** ($705–715 net). Neither needs lucky ads. “Best drag” below means acceleration; “best road speed” means the speed ceiling. Steering, rendering speed and opponents prevent a universal race-time guarantee.

All citations are repository paths and source lines at baseline `59f9f11`. Manifolds are identified by **numeric variant and catalog text**: variant **2 = Tri-Power**, variant **3 = Dual-Quad**; the C/MASM enum spellings currently interchange those names. No source was changed.

## Starting state and availability

- `src/GAME/SAVEGAME.H:6` defines `STARTING_MONEY 750L`; `SAVEGAME.C:121–132` calls `pl_init(1)`, assigns it, resets action time and win counts. `src/SYS/POOLS.C:28–34` clears both car lists, spare parts and ownership counts. **No starting car or parts.** Demo cash is $10,000, not the normal game.
- The **$400** at `GARAGE.H:19` is `BROKE_CASH_LIMIT`: game-over detection requires no car, no other cars, no spare parts, and cash below $400 (`GARAGE.C:261–266`). It is also the cheapest car's price.
- Action time starts at zero, with the running BIOS timer rebased (`SAVEGAME.C:130–131`). Day zero is **June 16, 1963**; day 91 is **September 15**, when time expires (`GARAGE.C:50–100,143–158`). There are **6,552 ticks/day**, 546 ticks/internal hour, **596,232 ticks** to the deadline (`GARAGE.H:11–14`). Thus one calendar day contains 12 internal hour units; do not interpret a service's “hours” as a 24-hour calendar day. The dashboard initially draws 9 o'clock (`DASH.C:256–260`). Ordinary time also advances while deciding: `SYS.C:868–880`; race timer: `SYS.C:1060`.
- A bought car gets **40 fuel units = four gallons**, zero engine/transmission/tire wear, and random timing **−8…+4** (`GAME.C:320–370`, `INPUTDAT.ASM:274–275`, `GARAGE.H:20–22`). Carb/manifold wear is the untracked sentinel **−128**. Prices and installed stock parts are fixed, not randomized used-car condition.

The resource is `orig/freeware/HOT_DATA`: CarSpec records are little-endian `<hBbHhh>` at **396 + 10×index**, models `<bbhhhh>` at **656 + 10×index**, description text starts at **1904**, and the ad head is the signed word at **3754 = 2**. These offsets follow `src/SYS/DISK.C:68–82,519–524` and `DISK.H:331–368`. The following table follows the actual ad chain. Equipment is **engine/transmission/manifold/carb/tire**, using numeric variants defined below; stock timing is random, so the speed column normalizes timing to zero and uses actual bought-car wear/body state.

| Spec | Model | Price | Rank | Next | Fit | Stock E/T/M/C/W | Stock rating mph |
| --- | --- | ---: | ---: | ---: | --- | --- | ---: |
| 2 | 1938 Chevrolet Master Deluxe 2-dr. | $1755 | 5 | 0 | GM | 2/2/3/1/1 | 121.27 |
| 0 | 1940 Chevrolet 2-dr coupe. | $400 | 0 | 1 | GM | 0/0/0/0/0 | 98.66 |
| 1 | 1940 Chevrolet Roadster. | $2865 | 7 | 16 | GM | 2/3/1/1/1 | 121.05 |
| 16 | 1949 Chevrolet 2dr Styleline. | $475 | 1 | 24 | GM | 1/0/0/0/0 | 102.70 |
| 24 | 1955 Chevrolet Bel-Air Sport coupe. | $950 | 2 | 17 | GM | 1/1/1/1/0 | 110.66 |
| 17 | 1958 Chevrolet Impala. | $1260 | 2 | 3 | GM | 1/0/0/0/1 | 102.84 |
| 3 | 1956 Corvette. | $1450 | 3 | 4 | GM | 1/2/1/1/0 | 116.63 |
| 4 | 1961 Corvette. | $2250 | 5 | 5 | GM | 2/2/1/1/1 | 121.86 |
| 5 | 1963 Corvette. | $3770 | 8 | 6 | GM | 2/3/4/2/2 | 127.66 |
| 6 | 1955 Dodge Custom Royal Lancer. | $625 | 0 | 8 | Chrysler | 1/0/1/1/0 | 104.90 |
| 8 | 1962 Dodge Polara. | $2050 | 6 | 7 | Chrysler | 1/1/0/0/0 | 109.15 |
| 7 | 1932 Ford. Customized deuce coupe. | $1700 | 4 | 10 | Ford | 1/2/1/1/1 | 115.64 |
| 10 | 1932 Ford Sedan. | $2680 | 7 | 11 | Ford | 2/2/3/1/1 | 124.32 |
| 11 | 1940 Ford Deluxe 2-dr. | $2450 | 6 | 12 | Ford | 1/2/1/1/1 | 114.01 |
| 12 | 1955 Ford Fairlane Victoria. | $1950 | 5 | 13 | Ford | 1/2/1/1/1 | 113.47 |
| 13 | 1957 Ford Fairlane 500 2-dr. | $640 | 1 | 14 | Ford | 1/1/0/0/0 | 107.56 |
| 14 | 1954 Mercury Monterey. | $900 | 1 | 15 | Ford | 1/1/1/1/1 | 110.48 |
| 15 | 1956 Mercury Custom 2-dr. | $455 | 0 | 18 | Ford | 0/0/0/0/0 | 98.66 |
| 18 | 1949 Olds 88 2dr.Customized. | $1850 | 4 | 19 | GM | 1/0/1/1/1 | 105.75 |
| 19 | 1952 Olds 88.  Customized. | $2055 | 6 | 9 | GM | 2/2/1/1/1 | 118.88 |
| 9 | 1962 Plymouth Savoy 2-dr. | $1200 | 2 | 20 | Chrysler | 1/0/0/0/0 | 102.84 |
| 20 | 1950 Pontiac Silver Streak. | $3100 | 7 | 21 | GM | 2/2/1/1/2 | 119.06 |
| 21 | 1957 T-Bird. | $3350 | 8 | 22 | Ford | 2/3/3/1/1 | 124.15 |
| 22 | 1961 Valiant V-200. | $1350 | 3 | 23 | Chrysler | 0/1/0/0/0 | 103.39 |
| 23 | 1963 Valiant V-100 2-dr. | $1575 | 4 | -1 | Chrysler | 2/1/1/1/0 | 116.24 |

The affordable stock choices are specs **0, 6, 13, 15, 16**. The King's spec **25** is excluded from the paper: price 0, rank 8, equipment 2/3/4/2/2, and all body-mod pictures zero. Its `nextAd=2` does not put it in the chain.

`GAME.C:452–463` follows `g_usedCarAdFirst` / `CarSpec.nextAd`, then appends **every catalog part valued at least $20**. There is **no daily ad regeneration, car deletion after purchase, stock depletion or price roll**: the only source uses of `nextAd` are traversal, and the head is only loaded/saved. The daily random newspaper illustration (`GAME.C:135–140`) does not change inventory. Parts can be bought repeatedly. Car purchases use `basePrice`, parts use `Item.value` (`GAME.C:469–505`); there is no purchase haggling.

Complete advertised parts/prices (`src/INPUT/INPUTDAT.ASM:141–270`; columns GM/Ford/Chrysler; “all” uses fit mask 7):

| Category / variant | GM | Ford | Chrysler | All manufacturers |
| --- | ---: | ---: | ---: | ---: |
| Engine 1: small V8, 283 / 292 / 285 cu in | $350 | $340 | $360 | — |
| Engine 2: large V8, 327 / 351 / 340 cu in | $500 | $475 | $525 | — |
| Transmission 0: automatic | $130 | $120 | $125 | — |
| Transmission 1: three-speed | $175 | $160 | $170 | — |
| Transmission 2: four-speed | $250 | $260 | $245 | $275 |
| Transmission 3: racing four-speed | $400 | $405 | $410 | — |
| Carb 0: two-barrel | — | — | — | $25 each |
| Carb 1: four-barrel | — | — | — | $40 each |
| Carb 2: racing four-barrel | — | — | — | $60 each |
| Manifold 0: two-barrel | $20 | $20 | $20 | — |
| Manifold 1: single four-barrel | $25 | $25 | $25 | — |
| Manifold 2: **Tri-Power** | $35 | $35 | $35 | — |
| Manifold 3: **Dual-Quad** | $35 | $35 | $35 | — |
| Manifold 4: racing | $60 | $60 | $60 | — |
| Tire 0: standard | — | — | — | $50 |
| Tire 1: brand-name | — | — | — | $80 |
| Tire 2: racing slicks | — | — | — | $150 |

V6 catalog entries 0/1/2 cost **$19**, below the threshold, so **cannot be bought from ads**; obtain one from a donor car or a pink-slip win. The terminator is also excluded. There are **40 advertised parts** (6 engines, 13 transmissions, 3 carbs, 15 manifolds, 3 tire sets).

## Fit and the performance calculation

Manufacturer compatibility is a bitwise intersection with the car's stock fit: **GM=1, Ford/Mercury=2, Chrysler=4, all=7** (`DISK.H:295–324`; transmission install `GARAGE.C:842`, engine/manifold/carb install `1127–1151`). A GM small/large engine fits the Styleline; a Ford engine does not. No additional displacement, chassis weight or physical gear ratio is consulted.

| Engine variant | Allowed manifolds | Manifold variant | Allowed carb variants | Number of carbs |
| --- | --- | --- | --- | ---: |
| 0 V6 | 0, 2 | 0 two-barrel | 0 | 1 |
| 1 small V8 | 0, 1, 3, 4 | 1 single four-barrel | 1 or 2 | 1 |
| 2 large V8 | 0, 1, 3, 4 | 2 Tri-Power | 0 | 3 |
| — | — | 3 Dual-Quad | 1 or 2 | 2 |
| — | — | 4 racing | 1 or 2 | 2 |

Tables: `INPUTDAT.ASM:284–294`; counts `GARAGE.C:753`. In particular, **Tri-Power fits the V6, not either V8**. The carb slots may mix permitted types; readiness requires all slots, connected wires and tight bolts, but not identical carbs (`GARAGE.C:909–931`). Installation works from engine → manifold → carbs; removal reverses that order (`1095–1208,1235–1325`).

Let `M` be the model score `[40,44,48,60,64,68,80,84,88][spec.rank]`; `E=[0,50,100][engine]`; `T=[0,33,66,100][transmission]`. Intake score **I is indexed by first carb variant, then manifold variant**, not a fit boolean:

| First carb \ manifold | 0 | 1 | 2 Tri-Power | 3 Dual-Quad | 4 racing |
| --- | ---: | ---: | ---: | ---: | ---: |
| 0 two-barrel | 0 | 0 | 100 | 0 | 0 |
| 1 four-barrel | 0 | 50 | 0 | 100 | 75 |
| 2 racing | 0 | 0 | 0 | 50 | 100 |

`PERF.C:11–24,48–62` and constants `PERF.H:6–20` give:

```text
R = .09*roof + .05*rear + .01*front
    + .10*M/88 + .15*I/100 + .25*E/100 + .15*T/100
    + .20*(1-abs(timing/8))
V = .7 + .3 * (automatic ? .8 : 1) * (1-engineWear/10000) * R
nominal_mph = min(130, 130*V)
```

Each body term is true when the part is removed/chopped **OR its model picture is zero**. Thus a car with an unavailable body operation gets that bonus free; the King gets all .15. Neither `Car.value` nor the owned `Car.rank` field enters this calculation: the rank is read from **CarSpec**. There is **no weight input**. Only **engine wear** reduces R; tire/transmission wear have other effects. Timing **zero** maximizes performance; no benefit to advancing beyond zero. Tuning costs no money, takes variable action time capped at 1,092 ticks, and clamps settings to −8…+4 (`GARAGE.C:935–1001`). Part removal/installation randomizes timing again (`1208,1320`), so **tune last**.

The rest of `pf_drivetrain` (`PERF.C:63–90`) creates the actual integer physics:

| Transmission | Gear speed percentages (neutral, then gears) | Gears |
| --- | --- | ---: |
| Automatic / three-speed | 0, 25, 50, 100, 0 | 3 |
| Four-speed | 0, 25, 50, 75, 100 | 4 |
| Racing four-speed | 0, 30, 50, 80, 100 | 4 |

For each gear `g`, `ratio[g] = int(5700/(percentage[g]*nominal_mph/100))`, with zero speed replaced by .2. Actual displayed speed is **integer `rpm/ratio[g]`**. The nominal rating is truncated only for the info screen (`pf_vmax`, `GARAGE.C:773`), not before calculating ratios.

| Engine | Base rpm steps, neutral / 1 / 2 / 3 / 4 | Low/mid boundary | Mid/high boundary |
| --- | --- | ---: | ---: |
| V6 | 500 / 600 / 300 / 100 / 70 | 2,900 | 4,500 |
| Small V8 | 600 / 700 / 350 / 120 / 90 | 2,900 | 5,000 |
| Large V8 | 600 / 760 / 380 / 140 / 90 | 2,500 | 5,200 |

With `v=int(base[engine][g]*V)`, the low/mid/high acceleration steps are:

```text
low  = v if neutral
       (v//5)*3 if first gear, slicks
       (v//5)*2 if first gear, other tires
       v//5 otherwise
mid  = v
high = v if neutral, else v//10
decel = v if neutral, else v//(5-g)
```

**Race physics:** `SYS.C:1060–1064,1150–1230` updates at BIOS rate/3, **6.068827333… updates/sec** (`RACE.H:17`), adds the selected acceleration step when accelerating, subtracts decel when coasting, and chooses the next band with equality remaining in the lower band. Redline is **5,700**, automatic cap **5,690**. A shift preserves the *previous integer speed*, resets rpm to `speed*new_ratio` (or **1,505** if zero), and discards that update's rpm acceleration. Revving in neutral therefore does not produce a high-rpm launch. Automatic upshifts use the engine shift boundary. Above the mid/high boundary acceleration falls to a tenth: shift near the boundary, considering whether the next gear lands below its low band. The large V8's **2,500-rpm** low boundary especially helps the three-speed's 2→3 shift; the small V8 drops into its weak low band.

Release the accelerator while upshifting in the power band: the reset already discards the acceleration, and the random manual-transmission failure condition requires **accelerator held and rpm >5,000** (`SYS.C:1182–1209`). Engine/transmission wear rises above 5,000 rpm; periodic engine wear also accrues. Wear ≥9,900 causes failure; repeatedly trying to exceed redline eventually blows the engine (`1150–1189`). Feather the accelerator at maximum speed instead of holding against redline.

The drivetrain is the **same for drag and road**. Ratios/steps are cached when leaving the garage (`GAME.C:832`), or switching to a higher-rank prize (`RACE.C:711`); ISR wear and post-race timing changes do **not** continuously recalculate V. Repeated races at Bob's can retain the earlier performance table until that next rebuild, although wear failures still apply. Slicks improve **first-gear low-band acceleration only**, without raising the speed rating. Brand-name tires and slicks both have zero fresh-tire lateral slip, versus standard tires' **2**; worn tire slip tables are `[2,5,8,11,14]`, `[0,2,4,6,8]`, `[0,0,1,2,4]` (`RACE.C:632,925–934`). Road lateral drift multiplies curve error by `distanceStep+tireSlip`; shoulders brake and roadside collisions end the race (`ROAD.C:975–997,1018–1038`, `SYS.C:1220–1224,1245–1247`). `gearRpmPerSpeed[5]` is filled from rank and tire factors 9/8/5 (`PERF.C:85–86`) but **has no consumer**; do not invent a handling bonus from it.

Distance is **render-loop driven**, not an integral of mph over ISR time: `DRIVE.C:127–134` → `DRVTICK.C:53–65` → `RACE.C:1206–1211`; `ROAD.C:916–918` carries fractions into indices. `ra_speed_step` has processor-rating-dependent speed buckets (`RACE.C:1147–1194`). On the faster branch its steps change at mph **13,27,41,55,69,83,97,101,105,109,113,117,121,125,129**. Drag is index **200→380**, road **200→1020** (`RACE.H:30–32`). The opponent also brakes/rubber-bands according to distance, CPU and driver penalties (`RACE.C:1065–1133`), and ordinary road races have a 1/4 cop roll (`989–993`). Consequently a drivetrain formula cannot establish one platform-independent drag ET or road winner. Reaction time, frame rate, shifts, steering and random failures matter; the figures below are **controlled drivetrain comparisons**, not measured finish times.

## Exhaustive calculation and the two opening builds

Ran `nix-shell shell.nix --run 'python3 work/fastcar.py'` in `~/wt/fastcar` on dagon. The scratch script decodes the shipped resource, parses the 43 catalog records from INPUTDAT, applies both fit matrices and manufacturer masks, and enumerates every engine/gearbox/manifold/**ordered carb tuple**/tire/available-body-bit configuration: **63,720 configurations, 63,360 ordinary-car configurations**. Donor-only V6 replacements are included in the performance envelope but assigned infinite *ad-purchase* cost. The remaining catalog states include **929 ordinary configurations with net opening cost ≤$750** using worst-case resale. Output: `work/fastcar-combinations.csv` (ratings, ratios, all acceleration bands and costs); `work/fastcar-output.txt` (rankings). All wear/timing values are parameters of the formula; the enumeration uses fresh parts and timing zero because those maximize the opening performance. It is not a search over repeated trading or race winnings.

For acceleration the script breadth-first searches full-throttle ISR states `(gear,rpm,integer_speed)`, permitting each manual upshift, with exact integer band/ratio transitions and automatic shifts. Starts from rest, counting one engagement tick then the searched first-gear trajectory. It ignores breakdown rolls and reaction delay; throttle release during power-band shifts can avoid the random shift-failure roll. This is an optimum over **monotone upshifts**, not a claim about every possible brake/downshift exploit. The full formula/data needed to repeat the enumeration are above; the scratch script is deliberately not part of this documentation-only commit.

| Build: Styleline unless stated | Net cost, worst resale | Nominal mph / info display | Attainable integer redline mph | Ideal launch→60 | Ideal launch→100 |
| --- | ---: | --- | ---: | ---: | ---: |
| Stock 283, automatic; timing corrected | $475 | 102.700 / 102 | 103 (automatic) | 66 ticks / 10.88 s | 133 ticks / 21.92 s |
| **Drag:** 327, 3-speed, single 4-barrel, both bumpers stripped | **$750** | 117.6955 / 117 | 118 | **21 / 3.46 s** | **37 / 6.10 s** |
| Same, only rear bumper stripped | $735 | 117.3055 / 117 | 118 | 21 / 3.46 s | 37 / 6.10 s |
| **Road speed:** 283, 3-speed, Dual-Quad + 2 ordinary 4-barrels, all body mods | **$715** | **119.2555 / 119** | **121** | 35 / 5.77 s | 56 / 9.23 s |
| Close-geared alternative: 283, GM 4-speed, single 4-barrel, all body mods | $740 | 118.261 / 118 | 118 | 27 / 4.45 s | 51 / 8.40 s |
| **Road grip:** 283, 3-speed, single 4-barrel, all body mods, brand-name tires | **$745** | 116.3305 / 116 | 118 | 41 / 6.76 s | 60 / 9.89 s |
| Racing gearbox alone: 283, stock intake/body/tires | $750 | 111.475 / 111 | 111 | 26 / 4.28 s | 58 / 9.56 s |

Drag build versus tuned stock: **+14.9955 nominal mph**, roughly **68% less launch→60 time** and **72% less launch→100 time** in this model. Road-speed build: **+16.5555 nominal mph**, **+18 displayed redline mph**, ~47% less launch→60 time. A random stock timing setting can make the actual as-bought comparison worse; these comparisons give stock the same optimal tune.

For completeness, best nominal rating at $750 for each other affordable chassis: **1940 Chevy 116.1337**, **1955 Dodge 115.1782**, **1957 Ford 115.3555**, **1956 Mercury 114.2032 mph**. The Styleline wins both the enumerated speed rating and launch→60/100 criteria. It does not follow that every possible frame-rate-dependent race is won fastest by the same build.

### Buy/build order and cash flow

Resale is a **separate sale**, not an automatic trade-in. At fresh wear engine/transmission proceeds are `floor5(cut + int((value-cut)*.9))`, `cut=int(value*.9)`; a V6 uses value **100**, not 19. Carb/manifold sales give **70–89%**, rounded down to $5; tires sell for **$0** (`GAME.C:174–182,232–270`). Therefore the Styleline's old **283 sells for $345**, automatic **$125**, two-barrel carb **$15–20**, manifold **$10–15**. Declining an offer rerolls it without the fixed sale-time cost, but real decision time still passes. Quoted worst-case builds do not need rerolls.

**Drag order (all cash balances below use minimum resale):**

1. Buy Styleline **$475** → **$275**. Disconnect wires; remove carb, manifold, then engine; remove automatic. Sell the four removed parts for **$495 total** → **$770**. This finances the large engine without needing $500 immediately after buying the car.
2. In one parts-paper visit buy GM **327 $500**, GM **three-speed $175**, GM **single four-barrel manifold $25**, and **one ordinary four-barrel carb $40** → **$30**. Reassemble engine → manifold → carb, tighten every bolt, reconnect wires, install/tighten gearbox. Keep the stock tires.
3. Strip both bumpers, **$15 each** → **$0**; **tune timing to 0 last**. Skip the roof. The variant-1 manifold scores 50 with an ordinary four-barrel, **zero with the more expensive racing carb**.
4. One minimum-time 100-mph trace shifts from **5,421 rpm / 28 mph** in first and **5,440 rpm / 56 mph** in second (release the accelerator on the shift updates). The rear removal alone costs **$735 net**, retaining **$15** for a cash wager and matching the listed 60/100 times. Both removals improve V slightly. New-car fuel is sufficient for either first race; pink-slip/free challenges do not require a cash stake. Do not buy more fuel before the first race unnecessarily.

**Road-speed order:** buy Styleline → $275; remove/sell automatic **$125**, carb **$15**, manifold **$10** → **$425**. Buy GM three-speed **$175**, **variant-3 GM Dual-Quad $35**, **two ordinary four-barrels $80** → **$135**. Install them, reconnect wires, chop roof **$70**, strip both bumpers **$30** → **$35**; tune to zero. Total **$715**, or **$705** with both intake sales at their best rolls. All required fits/counts are satisfied. First-gear ratios / second / third are **191 / 95 / 47**; its highest speed comes at the cost of a weak small-V8 2→3 transition. The four-speed alternative in the table is preferable when acceleration recovery matters more than the ceiling.

**Road grip option:** replace the Dual-Quad/two carbs with a **$25 single four-barrel manifold + one $40 carb**, and buy **$80 brand-name tires**, keeping all three body mods. Net **$745**, minimum $5 left; fresh slip drops **2→0**, while nominal mph drops to **116.3305**. This maximizes nominal rating among the affordable fresh brand-name-tire builds. Slicks cost $70 more, give no better fresh lateral slip, and divert money from power/body work. There is no numeric cornering-speed limit that proves a universal winner between grip and speed builds.

Body prices/time/effects (`GARAGE.H:15–18,25–46`; `GARAGE.C:470–528`): roof **$70 / 8,736 ticks / +.09 R**; rear **$15 / 2,184 / +.05 R**; front **$15 / 2,184 / +.01 R**. Those add **3.51 / 1.95 / .39 nominal mph** on a fresh manual car, less with automatic/wear. Body work does **not** increase `Car.value`; restoration costs again and reverses the bonus. Paint/stickers add no drivetrain performance; stickers can change challenge acceptance (`BOBS.C:479–487`).

| Minimum fixed action time | Drag, both bumpers | Road speed | Road grip |
| --- | ---: | ---: | ---: |
| Car + two paper openings | 2,184 | 2,184 | 2,184 |
| Part purchases / separate old-part sales | 2,184 / 4,368 | 2,184 / 3,276 | 2,184 / 3,276 |
| Gearbox removal + installation | 3,276 | 3,276 | 3,276 |
| Engine removal + installation | 3,276 | 0 | 0 |
| Manifold removal + installation | 1,638 | 1,638 | 1,638 |
| Carb removal + installations | 818 | 1,227 | 818 |
| Under-hood bolt work / wires | 1,080 / 218 | 1,080 / 218 | 864 / 218 |
| Tire installation | 0 | 0 | 1,092 |
| Body work | 4,368 | 13,104 | 13,104 |
| **Total before tuning / real elapsed time** | **23,410 ticks = 3.573 days** | **28,187 = 4.302 days** | **28,654 = 4.373 days** |

Times use `GARAGE.H:25–46`, sale/purchase additions in `GAME.C:269,496,504,821`, and actual remove/install paths `GARAGE.C:849,884,1076–1082,1132–1151,1235–1306`. Each under-hood bolt needs two removal clicks and two tightening clicks; installation inserts it automatically. Hood clicks each cost 54 ticks; transmission bolt clicks have no separate fixed charge. Disconnect/reconnect costs 109 each. Add tuning **0–1,092 ticks**, all actual elapsed BIOS ticks, and extra paper openings if purchases are split. Going to Bob's adds 546; a drag/road race adds **1,092 / 1,911**; returning adds another 546 (`GAME.C:841,859`, `BOBS.C:619–620`). These are lower bounds on elapsed calendar time, not promises of finishing on an exact date.

## Selling, racing for cars, and the eventual maximum

Whole-car sale uses the **mutable `Car.value`**, not performance or worn-part condition. Initial offer is **75–94%**; counteroffers and a final 90%-of-ask test affect acceptance, and asks above value are rejected (`GAME.C:648–700`). No direct swap/instant trade-in exists. Installing/removing parts adds/subtracts `gm_part_value`, not their sale proceeds (`GARAGE.C:650–666`). Ordinary whole-car flipping cannot sell above value; it is no shortcut to the $3,770 Corvette. Engine/intake/gearbox sales are what make the opening builds affordable.

Pink slips are the useful next step: acceptance depends on car class, engine variant, rival tolerance/history, stickers and a random roll (`BOBS.C:448–490`), so a particular target car is not guaranteed to appear/accept immediately. Winning gives a newly constructed stock car at **basePrice**, fresh engine/transmission, tire wear **0–499**, random timing and the rival's body mods; a higher-rank prize becomes current automatically (`RACE.C:695–725`). Retune it; sell unwanted prize cars/parts to finance upgrades. Losing a pink-slip race loses your car. Fuel costs 5 units/drag, 15/road; tire wear rises 300–599/700–999 on standard tires, 200–499/400–699 otherwise (`RACE.H:33–34`, `RACE.C:681–685,1001–1018`).

The King is **not an opening free Corvette**. With remaining rivals, his readiness logic effectively requires **at least six wager wins in each race type** and `wins >= (betRaces>>1)`; free races do not increment the normal win counts. Eliminating all ordinary rivals is an alternate readiness path (`BOBS.C:221–244,1071–1101`, constants `BOBS.H:81–83`). First beat him in a free drag, then in a pink-slip road race (`BOBS.C:522–539`). Winning the latter plays the ending and sets `END_KING_BEATEN` (`1081–1086`), so his spec-25 Corvette is not a normal playable upgrade afterwards.

With earnings, the mathematical maximum is a **rank-8 car, large V8, racing four-speed, 100-point intake, slicks, timing 0, fresh engine, all available body bonuses**: R=1, V=1, nominal **130 mph**, integer ratio 43 giving **132 mph** at 5,700 rpm. A normal **1957 T-Bird** already has the required engine, gearbox and Dual-Quad/ordinary-carb intake; **$3,350 + $150 slicks + $100 body work = $3,600** reaches that state. The normal 1963 Corvette needs only **$30 bumper work** beyond its $3,770 price. Rank-7 cars can also reach the same integer top-speed bucket with slightly lower acceleration. This is progression beyond the $750 opening, not an initially affordable purchase.

## Bugs and exploitable edges (separate from the recommended builds)

1. **Rating ≠ actual speed cap.** Truncated integer ratios allow **132 mph** even though the nominal formula caps at 130. The opening road build similarly rates 119.2555 but reaches displayed 121 (`PERF.C:60–69`, `SYS.C:1229–1230`). Maintaining redline with the accelerator pinned eventually blows the engine; this is an attainable peak, not indefinite full-throttle cruise.
2. **Only the first carb scores.** Two racing carbs on a racing manifold cost $120; a racing carb in slot 2 and an ordinary four-barrel in slot 3 cost $100 and give the same score. Swapping their order reduces it. All carbs must still exist and fit (`PERF.C:43–46,54`, `GARAGE.C:915–921`). Racing carbs on a single four-barrel manifold are legal but score zero.
3. **Spec-helper wear/body bug.** `pf_drivetrain_spec` / the spec branch of `pf_vmax` initialize *all* temporary engine-rack node wear to carb wear **−128**, including the engine, and leave `car.appearance` uninitialized (`PERF.C:92–119,124–165`). The engine multiplier can become **1.0128** and stack bits can award body bonuses. Opponent ratings can therefore differ from an actually bought stock car. The table above intentionally constructs the bought state; it does not trust these helpers as stock benchmarks.
4. **Negative shell-value / unsigned sale candidate, source analysis only.** Strip the Styleline's $350 engine, $130 transmission, $20 manifold and $25 carb: `475−350−130−20−25 = −50`. `remove_part` does not clamp value. `your_cars` converts negative offers and the 115%-of-value ask cap to unsigned; the latter becomes **65,479** for −50, permitting a huge ask through its numeric checks (`GARAGE.C:662–666`, `GAME.C:649,658–698`). Unlike `gm_scrap_car`'s final return (`GAME.C:419–422`), this path has no zero floor. **Not runtime-validated:** garage redraw/sale animation also dereference missing engine/part pointers (`GARAGE.C:334–336`, `GAME.C:585,707–715`), so this is not presented as a reliable money route. If successful it invalidates any literal “only $750 before the first race” restriction and funds the eventual maximum above.
5. **Donor resale mismatch.** The Styleline's fresh engine/gearbox/intake alone sell for **$495–505** against its **$475** purchase price, with the body/tires still left over. Repeated donor dismantling can grow cash, but consumes substantial action time and runs into the incomplete-car/value issues above. Likewise V6 valuation is $100 despite a $19 catalog entry. These are reasons to state the opening-build boundary explicitly, not assume all pre-race trading conserves the original $750.
6. **Misleading bumper dispatch names.** `GAME.C:925–930` sends `KR_GAR_REAR_BUMPER` to `BODY_FRONT_BUMPER` and vice versa. The formula uses the actual appearance flags (rear **0x8000**, front **0x4000**) and model pictures. Both-bumper recipes avoid the naming ambiguity; for the $735 option verify the **.05 rear flag** is the one removed.

Validation was source/data decoding, exhaustive enumeration and discrete-physics calculation; no emulator race was timed and no gameplay/source behavior was modified.
