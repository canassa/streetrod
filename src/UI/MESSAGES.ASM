        .MODEL MEDIUM

; Initialized data DS:239E..4718: the menu and message texts and their tables.
; Words and arrays at odd addresses: assembler data, not C definitions.

        .DATA
        PUBLIC  _g_txt
_g_txt DB      0, 0, '________________________', 0  ; DS:239E char[]
        DB      '________________________', 0
        DB      '________________________', 0
        DB      '________________________', 0
        DB      '________________________', 0
        DB      '________________________', 0
        DB      '________________________', 0
        DB      '________________________', 0
        DB      'New Game', 0
        DB      'Old Game', 0
        DB      'Save Game', 0
        DB      'Load Game', 0
        DB      'OK', 0
        DB      'No', 0
        DB      'No thanks', 0
        DB      'No saved games found !', 0
        DB      'Disk error during Save', 0
        DB      'Cannot read that game !', 0
        DB      'Yeah', 0
        DB      'Skip it', 0
        DB      'See it', 0
        DB      'Offer', 0
        DB      'Never mind', 0
        DB      'Forget it', 0
        DB      'Quit', 0
        DB      'Previous color', 0
        DB      'Next color', 0
        DB      'Go ahead', 0
        DB      'Fix it', 0
        DB      'Junk it', 0
        DB      'Done', 0
        DB      'Tune', 0
        DB      'Select', 0
        DB      'Parts', 0
        DB      'Save, Restart, or Quit.', 0
        DB      'Your cars', 0
        DB      'Car parts', 0
        DB      'Spare parts', 0
        DB      'Switch it', 0
        DB      'Sell it', 0
        DB      'Drag race', 0
        DB      'Road race', 0
        DB      'Just``for``kicks', 0
        DB      '$10', 0
        DB      '$50', 0
        DB      '$25', 0
        DB      '$100', 0
        DB      'Pink slips', 0
        DB      '[', 0
        DB      '\', 0
        DB      '^', 0
        DB      ']', 0
        DB      'Previous page', 0
        DB      'Next page', 0
        DB      'Auto Parts', 0
        DB      'Used Cars', 0
        DB      'Go to the garage', 0
        DB      'Go to the garage', 0
        DB      'Hit the street', 0
        DB      'Check out the newspaper', 0
        DB      'Wanna drink ?', 0
        DB      'Cars', 0
        DB      'Sell parts', 0
        DB      'Change tires', 0
        DB      'Change transmission', 0
        DB      'Time to quit', 0
        DB      'Pop the hood', 0
        DB      'Catch some tunes', 0
        DB      'Squelch it !', 0
        DB      'New paint job - $20', 0
        DB      'Save it', 0
        DB      'Front bumper', 0
        DB      'Rear bumper', 0
        DB      'The roof', 0
        DB      'Stickers - $5', 0
        DB      'Wanna tank ?', 0
        DB      'Car info', 0
        DB      'Calendar', 0
        DB      "I'll take $", 0
        DB      'Retard', 0
        DB      'Advance', 0
        DB      'The old game will be replaced !', 0
        DB      'A bolt', 0
        DB      'Wires', 0
        DB      'The pump', 0
        DB      'The rear windshield', 0
        DB      'The front windshield', 0
        DB      'A rag', 0
        DB      'A tank cap', 0
        DB      'Get out of my face', 0
        DB      'Looking for', 0
        DB      0, 'Back to the garage', 0
        DB      'Click to challenge', 0
        DB      'Order', 0
        DB      'Look for opponent', 0
        DB      'Grey', 0
        DB      'Brown', 0
        DB      'Red', 0
        DB      'Pink', 0
        DB      'Orange', 0
        DB      'Yellow', 0
        DB      'Dark green', 0
        DB      'Light green', 0
        DB      'Dark blue', 0
        DB      'Light blue', 0
        DB      'Dark purple', 0
        DB      'Light purple', 0
        DB      'Call', 0
        DB      'Chop roof - $70', 0
        DB      'Rebuild roof - $70', 0
        DB      'Strip bumper - $15', 0
        DB      'Restore bumper - $15', 0
        PUBLIC  _g_msgWannaQuit
_g_msgWannaQuit DB      'Wanna quit ?', 0        ; DS:28EC char[]
        PUBLIC  _g_msgBackToGarage
_g_msgBackToGarage DB      'Back to the garage', 0  ; DS:28F9 char[]
        PUBLIC  _g_msgClickToEdit
_g_msgClickToEdit DB      'Click here to edit', 0  ; DS:290C char[]
        PUBLIC  _g_boxText
_g_boxText DB      31 DUP (0)               ; DS:291F char[]
        PUBLIC  _g_promptLine1
_g_promptLine1 DB      41 DUP (0)               ; DS:293E char[]
        PUBLIC  _g_promptLine2
_g_promptLine2 DB      41 DUP (0)               ; DS:2967 char[]
; DS:2990 char[]
        PUBLIC  _g_offerName
_g_offerName DB      0, ',,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,', 0
        DB      'You want to buy it for '
        PUBLIC  _g_buyPrice
_g_buyPrice DB      0, '----------', 0       ; DS:29D6 char[]
        DB      'How about ', 0
        DB      '          ', 0
        PUBLIC  _g_offerAmount
_g_offerAmount DB      0, '     ', 0            ; DS:29F8 char[]
; DS:29FF char[]
        PUBLIC  _g_carName
_g_carName DB      0, '.............................................', 0
        PUBLIC  _g_saveName
_g_saveName DB      0, '              ', 0   ; DS:2A2E char[]
        PUBLIC  _g_msgSellChange
_g_msgSellChange DB      'Change', 0              ; DS:2A3E char[]
        DB      0, 0, 0, 0, 'Forget it', 0
        DB      0, 'OK', 0
        DB      8 DUP (0)
; DS:2A5F char[]
        PUBLIC  _g_salePartName
_g_salePartName DB      0, '222222222222222222222222222222222222222222222', 0
        DB      'Oh, well, it could have been worse.', 0
; DS:2AB2 char[]
        PUBLIC  _g_msgRepairCost
_g_msgRepairCost DB      '$123 for``parts``and``you``are``back``on``the``stree'
        DB      'ts.', 0
        PUBLIC  _g_msgPlusTires
_g_msgPlusTires DB      31 DUP (0)               ; DS:2AEA char[]
; DS:2B09 char[]
        PUBLIC  _g_msgRunCost
_g_msgRunCost DB      "Damn.``It'll``cost $123 to``get``this``baby``running"
        DB      ".", 0
        PUBLIC  _g_raceResult1
_g_raceResult1 DB      51 DUP (0)               ; DS:2B3F char[]
        PUBLIC  _g_raceResult2
_g_raceResult2 DB      51 DUP (0)               ; DS:2B72 char[]
        PUBLIC  _g_offBolt
_g_offBolt DW      3E9h                     ; DS:2BA5 int
        PUBLIC  _g_offWires
_g_offWires DW      3F0h                     ; DS:2BA7 int
        PUBLIC  _g_offSquelch
_g_offSquelch DW      336h                     ; DS:2BA9 int
        PUBLIC  _g_offTunes
_g_offTunes DW      325h                     ; DS:2BAB int
; DS:2BAD int[]
        PUBLIC  _g_groupHot
_g_groupHot DW      29h, 0, 2, 6, 1Ah, 2Eh, 43h, 48h, 4Dh, 6Ch, 73h, 87h
        DW      9Ch, 0A3h, 0A6h, 0ADh, 0B2h, 0B7h, 0B9h, 0BFh, 0C5h
        DW      0CBh, 0DDh, 0E2h, 0EAh, 0EDh, 0F4h, 0FAh, 102h, 107h
        DW      10Ch, 111h, 116h, 11Bh, 11Fh, 124h, 12Fh, 142h, 15Dh
        DW      149h, 156h, 158h
        PUBLIC  _g_descHdr
_g_descHdr DB      0                        ; DS:2C01 char[]
        PUBLIC  _g_desc
_g_desc DB      1851 DUP (0), 1, 'GM V-6 engine', 0  ; DS:2C02 char[]
        DB      1, 'Ford V-6 engine', 0
        DB      1, 'Chrysler V-6 engine', 0
        DB      2, 'GM V-8 engine. 283 cu.in.', 0
        DB      '+ A clean', 0
        DB      'machine. $350', 0
        DB      2, 'Ford V-8 engine. 292 cu.in.', 0
        DB      '+``Never', 0
        DB      'raced. $340', 0
        DB      2, 'Chrysler V-8 eng 285 cu.in.', 0
        DB      '+ Like', 0
        DB      'new. $360', 0
        DB      2, 'GM V-8 engine. 327 cu.in.', 0
        DB      '+ Low', 0
        DB      'mileage. $500', 0
        DB      1, 'Ford V-8 eng 351 cu.in.', 0
        DB      '+ $475', 0
        DB      2, 'Chrysler V-8 eng 340 cu.in.', 0
        DB      '+``Just', 0
        DB      'rebuilt. $525', 0
        DB      1, 'Auto Trnsmsn GM.', 0
        DB      '+ $130', 0
        DB      1, 'Auto Trnsmsn Ford', 0
        DB      '+ $120', 0
        DB      1, 'Auto Trnsmsn Chrysler.', 0
        DB      '+ $125', 0
        DB      1, 'Trnsmsn 3-spd GM.', 0
        DB      '+ $175', 0
        DB      1, 'Trnsmsn 3-spd Ford.', 0
        DB      '+ $160', 0
        DB      1, 'Trnsmsn 3-spd Chrysler.', 0
        DB      '+ $170', 0
        DB      1, 'Trnsmsn 4-spd. GM.', 0
        DB      '+ $250', 0
        DB      1, 'Trnsmsn 4-spd. Ford.', 0
        DB      '+ $260', 0
        DB      1, 'Trnsmsn 4-spd. Chrysler.', 0
        DB      '+ $245', 0
        DB      1, 'Trnsmsn 4-spd. Fits all.', 0
        DB      '+ $275', 0
        DB      1, 'Racing 4-spd. GM.', 0
        DB      '+ $400', 0
        DB      1, 'Racing 4-spd. Ford.', 0
        DB      '+ $405', 0
        DB      1, 'Racing 4-spd. Chrysler.', 0
        DB      '+ $410', 0
        DB      1, '2-brl. carb. Fits all.', 0
        DB      '+ $25', 0
        DB      1, '4-brl. carb. Fits all.', 0
        DB      '+ $40', 0
        DB      2, '4-brl racing carb. Fits all models.', 0
        DB      'Chrome. $60', 0
        DB      1, 'GM 2-brl manifold.', 0
        DB      '+ $20', 0
        DB      1, 'Ford 2-brl manifold.', 0
        DB      '+ $20', 0
        DB      1, 'Chrysler 2-brl manifold.', 0
        DB      '+ $20', 0
        DB      1, 'GM 4-brl manifold.', 0
        DB      '+ $25', 0
        DB      1, 'Ford 4-brl manifold.', 0
        DB      '+ $25', 0
        DB      1, 'Chrysler 4-brl manifold.', 0
        DB      '+ $25', 0
        DB      1, 'GM manifold.`For 2-4brl.carbs.', 0
        DB      '+ $35', 0
        DB      1, 'Ford manifold.`For``2-4brl.carbs.', 0
        DB      '+`$35', 0
        DB      2, 'Chrysler manifold.`For``2-4brl.carbs.', 0
        DB      '$35', 0
        DB      1, 'GM manifold. For 3-2brl.carbs.', 0
        DB      '+$35', 0
        DB      1, 'Ford manifold.`For``3-2brl.carbs.', 0
        DB      '+$35', 0
        DB      2, 'Chrysler manifold.`For``3-2brl.carbs.', 0
        DB      '$35', 0
        DB      1, 'GM racing manifold.', 0
        DB      '+ $60', 0
        DB      1, 'Ford racing manifold.', 0
        DB      '+ $60', 0
        DB      1, 'Chrysler racing manifold.', 0
        DB      '+ $60', 0
        DB      1, 'Tires.', 0
        DB      '+ Four for $50', 0
        DB      2, 'Tires. Brand name.', 0
        DB      '+ All sizes. Four', 0
        DB      'for only $80', 0
        DB      2, 'Racing slicks.', 0
        DB      '+``All sizes.', 0
        DB      'Four for $150', 0
        DB      3, ' ', 0
        DB      '---------- USED CARS ----------', 0
        DB      ' ', 0
        DB      3, ' ', 0
        DB      '------------ PARTS ------------', 0
        DB      ' ', 0
        DB      1, "It's yours`!", 0
        DB      3, "You've got no room for all these cars`!", 0
        DB      "You're gonna hafta sell one.", 0
        DB      3, "You've got``too``much``junk``in``your``garage`!", 0
        DB      "You're gonna hafta sell a part.", 0
        DB      3
; DS:39C9 char[]
        PUBLIC  _g_junk
_g_junk DB      "You've got``too``much``junk``in``your``garage`!", 0
        DB      'Must``sell``12``parts``to``make``enough``room.', 0
        DB      1, 'Sell some cars first`!', 0
        DB      1, 'Sell some parts first`!', 0
        DB      2
        PUBLIC  _g_msgShort
_g_msgShort DB      "No go, hot shot. You're $12345 short.", 0  ; DS:3A5A char[]
        PUBLIC  _g_noRunHdr
_g_noRunHdr DB      2                        ; DS:3A80 char
S_3a81  DB      "Your car's not running, speedy`!", 0  ; DS:3A81 string
        PUBLIC  _g_noTuneHdr
_g_noTuneHdr DB      2                        ; DS:3AA2 char
S_3aa3  DB      "Can't tune that mess, klutz`!", 0  ; DS:3AA3 string
        PUBLIC  _g_kiddingHdr
_g_kiddingHdr DB      1                        ; DS:3AC1 char
S_3ac2  DB      'You must be kidding`!', 0  ; DS:3AC2 string
        PUBLIC  _g_partsGoneHdr
_g_partsGoneHdr DB      1                        ; DS:3AD8 char
S_3ad9  DB      'All parts gone`!', 0    ; DS:3AD9 string
        PUBLIC  _g_carsGoneHdr
_g_carsGoneHdr DB      1                        ; DS:3AEA char
S_3aeb  DB      'All cars gone`!', 0     ; DS:3AEB string
        PUBLIC  _g_noCarHdr
_g_noCarHdr DB      2                        ; DS:3AFB char
S_3afc  DB      "You've got no car, dummy`!", 0  ; DS:3AFC string
        PUBLIC  _g_pickCarHdr
_g_pickCarHdr DB      1                        ; DS:3B17 char
S_3b18  DB      'Pick a car first`!', 0  ; DS:3B18 string
        PUBLIC  _g_noSparesHdr
_g_noSparesHdr DB      2                        ; DS:3B2B char
S_3b2c  DB      "You've got no spare parts, dummy`!", 0  ; DS:3B2C string
        PUBLIC  _g_noTiresHdr
_g_noTiresHdr DB      2                        ; DS:3B4F char
S_3b50  DB      "You've got no spare tires, dummy`!", 0  ; DS:3B50 string
        PUBLIC  _g_noTransHdr
_g_noTransHdr DB      2                        ; DS:3B73 char
S_3b74  DB      "You've got no spare transmissions`!", 0  ; DS:3B74 string
        PUBLIC  _g_noEngineHdr
_g_noEngineHdr DB      2                        ; DS:3B98 char
S_3b99  DB      "You've got no spare engine`!", 0  ; DS:3B99 string
        PUBLIC  _g_noCarbHdr
_g_noCarbHdr DB      2                        ; DS:3BB6 char
S_3bb7  DB      "You've got no spare carburator`!", 0  ; DS:3BB7 string
        PUBLIC  _g_noManifoldHdr
_g_noManifoldHdr DB      2                        ; DS:3BD8 char
S_3bd9  DB      "You've got no spare manifold`!", 0  ; DS:3BD9 string
        PUBLIC  _g_howAboutHdr
_g_howAboutHdr DB      1                        ; DS:3BF8 char
        PUBLIC  _g_msgHowAbout
_g_msgHowAbout DB      'How about $1234 ?', 0   ; DS:3BF9 char[]
        DB      1
        PUBLIC  _g_msgWillYouTake
_g_msgWillYouTake DB      'Will you take $1234 ?', 0  ; DS:3C0C char[]
        DB      2, "I'll take it off your hands for nothing.", 0
        DB      2, "You must be crazy. I'll take it`!", 0
        DB      1, 'No way`!', 0
        DB      1, 'Get lost`!', 0
        DB      1, 'No thanks`!', 0
        DB      1, "I'll take it`!", 0
        DB      1, "Paint ain't free`!", 0
        DB      1, 'Pay attention, dummy`!', 0
        DB      2, "Pay attention, dummy`! That won't fit`!", 0
        DB      1, 'Get out of my face, dummy`!', 0
        DB      2, "I don't got time for amateurs like you`!", 0
        DB      3, "You're doing OK kid, but you're gonna", 0
        DB      'need to win more races`!', 0
        DB      2, "You're gonna hafta win more drag races`!", 0
        DB      2, "You're gonna hafta win more road races`!", 0
        DB      2, "You're on! Let's hit the drag race`!", 0
        DB      3, 'Cool, dude`! Get your pink slip out,', 0
        DB      "and let's go`!", 0
        DB      1, 'Chicken`!', 0
        DB      1, 'Get lost`!', 0
        DB      1, 'Give me a break`!', 0
        DB      1, 'Get outta my face`!', 0
        DB      1, 'Buzz off`!', 0
        DB      1, 'Whadda loser`!', 0
        DB      1, "You're on`!", 0
        DB      1, 'Alright`!', 0
        DB      1, "Let's go`!", 0
        DB      1, 'Blow it out your ear`!', 0
        DB      1, 'Forget it, geek`!', 0
        DB      1, 'Beat it, twerp`!', 0
        DB      1, 'No way, ya nerd`!', 0
        DB      1, 'Cool !', 0
        DB      1, 'O.K. !', 0
        DB      1, "Let's hit it`!", 0
        DB      2, "You``don't``have``enough``dough``for``that``bet`!"
        DB      0
        DB      2, 'Ya jumped the light, speedy`!', 0
        DB      2, "You won! Too bad you didn't bet, though.", 0
        DB      2, 'Big deal. Ten lousy bucks.', 0
        DB      2, 'Big deal. Twenty-five bucks.', 0
        DB      2, 'Fifty smackers. Not bad.', 0
        DB      2, 'A hundred smackers, hot shot.', 0
        DB      2, 'You just won a helluva car.', 0
        DB      1, 'You lose, sucker.', 0
        DB      1, 'Take a walk, sucker.', 0
        DB      2, 'Close, but no cigar punk`!', 0
        DB      3, "I`knew`you'd`choke`when`the`pressure`got`to`ya.", 0
        DB      'Better luck next time, sucker`!', 0
        DB      2, "Beginner's luck, punk`!", 0
        DB      3, 'You ran a good race.', 0
        DB      'You deserve to be King of the County`!', 0
        DB      3
        PUBLIC  _g_msgTotaled
_g_msgTotaled DB      "Lucky you weren't killed, 'cause the car", 0  ; DS:4115 char[]
        DB      'is totaled. You get $123 for scrap.', 0
        DB      3, "Lucky you're so slow. You only lost", 0
        DB      'the race.', 0
        DB      3, "Lucky you're so slow. You only lost", 0
        DB      'the race. But you need new tires.', 0
        DB      3, 'Too bad. You not only lost,', 0
        DB      'you need new tires.', 0
        DB      2, 'You just dropped your`transmission,klutz`!', 0
        DB      2, 'Blew your engine again, huh, Ace`?', 0
        DB      2, 'You lose. Take a walk, sucker.', 0
        DB      3, 'You just dropped your`transmission`!', 0
        DB      'You lose. Take a walk, sucker.', 0
        DB      3, 'Blew your engine again`?', 0
        DB      'You lose. Take a walk, sucker.', 0
        DB      3, 'Blew your tires again`?', 0
        DB      'You lose. Take a walk, sucker.', 0
        DB      3, "You don't got enough gas to cruise down", 0
        DB      "to Bob's.", 0
        DB      3, "You're gonna hafta get more gas", 0
        DB      'before you race.', 0
        DB      2, "You've been sentenced for debts, Ace`!", 0
        DB      3, 'Looks like ya took too much time, jr`!', 0
        DB      "You're done fer the summer.", 0
        DB      3, "You're outta dough.", 0
        DB      "Looks like you're done for the summer`!", 0
        DB      2, "I knew you could do it`! You're my hero`!", 0
        DB      1, 'Here it is !', 0
        DB      3, 'Where will you put it, speedy`?', 0
        DB      'Make some room first !', 0
        DB      2, 'You need``a``better graphics card``to``paint`!', 0
        DB      1, "It's not full, speedy`!", 0
        DB      2, "Don't drink and drive, bub`!", 0
        DB      2, "Ya can't chop this roof, dummy`!", 0
        DB      2, "You've already lost your bumper`!", 0
        DB      1, "Customization ain't free`!", 0
        DB      1, "Stickers ain't free`!", 0
        DB      1, "What's his name`?", 0
        DB      1, "Gas ain't free`!", 0
        DB      1, "You've got a full tank`!", 0
        DB      1, 'Your tank is empty`!', 0
        DB      2, 'Yo, you need full service, dummy`!', 0
        DB      1
        PUBLIC  _g_blankName
_g_blankName DB      '_______________', 0     ; DS:4607 char[]
        DB      1, '012345678901234', 0
        DB      1, '012345678901234', 0
        DB      1, '012345678901234', 0
        DB      1, '012345678901234', 0
        DB      1, '012345678901234', 0
        DB      1, '012345678901234', 0
        DB      1, '012345678901234', 0
        DB      1, '012345678901234', 0
        DB      1, '012345678901234', 0
        DB      1, '012345678901234', 0
        DB      1, '012345678901234', 0
        DB      1, '012345678901234', 0
        DB      1, '012345678901234', 0
        DB      1, '012345678901234', 0
        DB      1, '012345678901234', 0
        DB      0, 0

        END
