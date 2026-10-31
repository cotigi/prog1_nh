# Specifikáció:

## Felhasználói Felület:
Az Audió Szintetizátor alapvetően egy SDL3 GUI-n keresztül lesz 
elérhető, ahol az alábbi műveleteket lehet elvégezni:

- MIDI fájl kiválasztás 
- Oszcillátor konfiguráció kiválasztás
- Oszcillátor paraméterek módosítása (_waveform_, _velocity_, ...)
- Audió fájl lejátszása

> [!note] Megjegyzés:
> SDL3 főleg a megújult audió lejátszás miatt lett választva.

## MIDI Fájl:
A program egy adott MIDI fájl alapján fogja le szintetizálni 
az audió fájl-t.

> [!note] Megjegyzés:
> Vannak MIDI eventek amikre nem lesz szükség a jelenlegi
> projekt keretein belül. 
> Erre néhány példa:
> - Control/Mode Change
> - Program Change
> - Pitch Bend Change
>
> [MIDI eventek](https://midi.org/expanded-midi-1-0-messages-list)

## Oszcillátor:
Minden MIDI channel kap egy saját oszcillátort aminek az 
alap állása egy Sine waveform, konstans velocity-vel.
A GUI-n keresztül lehetőség lesz az adott oszcillátor
paramétereinek módosítására, a jelenlegi paraméterek 
elmentésére és az addig elmentett konfigurációk beolvasására.

### Konfigurációs Fájl:
Az oszcillátorok paramétereinek tárolása [INI](https://en.wikipedia.org/wiki/INI_file)
fájlok-al fog történni, a könnyű olvashatóság és módosítás illetve kezelhető beolvasás
érdekében.

### Paraméterek:
- Waveform
- Velocity 
- Amplitude

### Waveform-ok:
Alapvetően csak 3 hullám tipus lesz:
- Sine wave
- Square wave
- Triangle wave
