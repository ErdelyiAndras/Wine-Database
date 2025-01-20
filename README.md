# ELTE IK PTI Operációs-rendszerek 24-25-1 beadandó

### Első beadandó

*"Borban az igazság, borban a vigasz"*. Felfutóban van a minőségi bortermelés. A *"Tokaji Hegyvidék"* bírója (hegybíró) elrendelte, hogy a pontosabb nyilvántartás és a nemkívánatos *"csinált bor"* háttérbe szorítása miatt minden borászat köteles jelenteni az egyes szüretelt fajták eredményeit. Azaz meg kell adni, hogy melyik borászat, milyen típusú szőlőt szüretelt, amiből hány liter, milyen cukorfokú must készült. Például: Öt puttony borászat, kékfrankos szőlő, 2960 liter, 21 cukorfokú mustot készített.

A szüreti eredmény adatokat fájlban tároljuk, az adatfelvételen túl legyen lehetőségünk az adatokon módosítani, törölni vagy listát készíteni.

Készítsen C nyelvű programot, ami segít a hegybírónak, és ezt a feladatot megoldja, a megoldásnak vagy az opsys.inf.elte.hu kiszolgálón, vagy egy hozzá hasonló (linux) rendszeren kell futnia.

### Második beadandó

*"..., Borban a felejtés, Borban a tavasz"* folytatja az ismert dal. Bár a tavasz még odébb van, így előtte a *"Tokaji Hegyvidék"* bírója (hegybíró - szülő) elrendelte, hogy a nemkívánatos *"csinált bor"* háttérbe szorítása miatt minden borászat köteles jelenteni az egyes szüretelt fajták eredményeit. Ismert, hogy melyik borászat, milyen típusú szőlőt szüretelt, amiből hány liter, milyen cukorfokú must készült. Hegybíró mint mindig, az aktuális időjárási viszonyokat figyelembe véve, rendelkezik egy szőlő fajtára jellemző cukorfok értékkel. Például: Tokaji furmint: 21, Tokaji guggolós: 17, Kékfrankos: 19, stb. A must ellenőrzésekhez véletlenszerűen kirendel egy bizottságot (gyerek folyamat). A bizottság ha készen áll a munkára, jelzést küld a hegybírónak, aki erre válaszul elküldi az ellenőrizendő adatokat és a hozzá tartozó irányadó mustfokot csövön keresztül. Például: Öt puttony borászat, kékfrankos szőlő, 2960 liter, 21 cukorfokú mustot készített, irányadó mustfok: 19

Ha az irányadó mustfoktól nagyobb az eltérés mint 1 fok, akkor a bizottság csövön visszaírja hegybírónak, hogy a vizsgálat gyanúsnak találta a borászatot és a bizottság (gyerek) befejezi tevékenységét. Hegybíró, ezt az eredményt képernyőre írja, majd készen áll az esetleges további ellenőrzések elrendelésére.

Készítsen C nyelvű programot ami segít a hegybírónak, és ezt a feladatot megoldja, a megoldásnak vagy az opsys.inf.elte.hu kiszolgálón, vagy egy hozzá hasonló (linux) rendszeren kell futnia.
