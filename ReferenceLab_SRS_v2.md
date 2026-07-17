# ReferenceLab – Software Requirements Specification (SRS)

**Versione documento:** 2.0  
**Stato:** Bozza di progettazione approvata  
**Autore:** Riccardo  
**Data:** Luglio 2026  
**Tecnologie principali:** C++20, JUCE, VST3  
**Piattaforma iniziale:** Windows 10/11  

---

# 1. Introduzione

## 1.1 Scopo del documento

Questo documento definisce i requisiti software di **ReferenceLab**, un plugin VST3 progettato per il confronto A/B tra il segnale audio proveniente dalla DAW e una o più tracce audio di riferimento.

Il documento descrive:

- obiettivi del prodotto;
- ambito funzionale;
- architettura logica;
- requisiti funzionali;
- requisiti non funzionali;
- modello dati;
- interfacce interne;
- casi d'uso;
- criteri di accettazione;
- piano di test;
- roadmap fino alla versione 2.

Lo scopo è fornire una base tecnica sufficientemente precisa per iniziare l'implementazione senza dover ridefinire l'architettura principale durante lo sviluppo.

## 1.2 Visione del prodotto

ReferenceLab deve diventare un ambiente professionale per il confronto tra mix e reference track.

Il plugin non deve limitarsi alla semplice riproduzione A/B, ma deve consentire all'utente di:

- confrontare due sorgenti allo stesso loudness percepito;
- isolare porzioni dello spettro;
- confrontare contenuto stereo, mono, Mid e Side;
- visualizzare contemporaneamente parametri tecnici delle due sorgenti;
- organizzare una libreria di reference;
- aggiungere e modificare metadati;
- usare in futuro sorgenti locali e online tramite un'architettura estendibile.

## 1.3 Ambito della versione 1

La versione 1 deve supportare esclusivamente reference locali.

Le funzioni principali sono:

- caricamento di file audio locali;
- organizzazione della libreria;
- modifica e salvataggio metadati;
- confronto A/B;
- loudness matching;
- loop di una sezione;
- analizzatori;
- equalizzazione di confronto;
- modalità Stereo, Mono, Mid e Side;
- disattivazione automatica durante il rendering offline;
- cache delle reference;
- memorizzazione di offset e punti di riproduzione.

## 1.4 Ambito della versione 2

La versione 2 estende il sistema con:

- sorgenti HTTP;
- cataloghi JSON esterni;
- catalogo Cambridge MT, previa verifica tecnica e legale;
- gestione di più librerie;
- playlist;
- sincronizzazione avanzata con il transport della DAW;
- waveform completa;
- provider di reference modulari;
-Miglioramenti dell’interfaccia grafica e delle visualizzazioni

La versione 2 deve introdurre un’interfaccia grafica più chiara, coerente e flessibile, con particolare attenzione alla leggibilità delle informazioni e alla velocità del confronto tra Mix e Reference.

#### Visualizzazione dello spettro e della curva EQ

Il pannello dello spettro deve mostrare contemporaneamente:

* lo spettro del Mix;
* lo spettro della Reference;
* la curva di risposta dell’EQ di confronto;
* l’effetto complessivo dei filtri High Pass, Band Pass e Low Pass;
* una griglia con valori numerici sugli assi;
* una legenda delle sorgenti e degli elementi visualizzati.

L’asse orizzontale deve rappresentare la frequenza in Hz, preferibilmente con scala logaritmica, mostrando valori di riferimento come:

* 20 Hz;
* 50 Hz;
* 100 Hz;
* 200 Hz;
* 500 Hz;
* 1 kHz;
* 2 kHz;
* 5 kHz;
* 10 kHz;
* 20 kHz.

L’asse verticale deve mostrare il livello in dB e, quando l’EQ è visualizzato, il relativo guadagno o attenuazione.

Il pannello Analysis deve includere un controllo rotativo **Slope** per compensare visivamente l'inclinazione degli spettri FFT. Il controllo deve avere un intervallo inclusivo da **0,0 a 4,5 dB/ottava**, un valore predefinito di **4,5 dB/ottava** e una risoluzione massima di **0,1 dB/ottava**. Il valore corrente deve essere sempre leggibile accanto al pomello.

La compensazione Slope deve essere applicata in modo identico agli spettri Mix, Reference e agli eventuali spettri medi o congelati. La modifica deve aggiornare immediatamente i grafici, anche durante il Freeze, senza modificare il segnale audio, i dati FFT originali o la curva di risposta dell'EQ.

La curva dell’EQ deve aggiornarsi dinamicamente durante la modifica dei parametri e deve permettere di comprendere immediatamente quali frequenze vengono isolate, attenuate o enfatizzate.

La legenda deve indicare chiaramente almeno:

* Mix;
* Reference;
* curva EQ;
* eventuale spettro medio o congelato.

#### Coerenza cromatica

A ogni sorgente deve essere assegnato un colore principale coerente in tutta l’interfaccia.

Esempio:

* Mix: azzurro;
* Reference: arancione.

Il colore assegnato deve essere utilizzato per tutti gli elementi associati alla sorgente, inclusi:

* spettro FFT;
* waveform;
* meter;
* valori LUFS;
* Peak;
* RMS;
* Crest Factor;
* Dynamic Range;
* Correlation Meter;
* Stereo Width;
* indicatori e label;
* eventuali marker o bordi dei pannelli.

I colori devono essere configurabili dalle impostazioni del plugin.

La distinzione tra Mix e Reference non deve dipendere esclusivamente dal colore. Ogni elemento deve includere anche un’etichetta, un simbolo o uno stile grafico riconoscibile.

#### Waveform completa e marker della Reference

Il pannello della Reference deve mostrare la forma d’onda completa del file audio.

Sopra la waveform devono essere visualizzati i seguenti marker:

* **Starting Point:** punto iniziale della riproduzione o del loop;
* **Current Point:** posizione corrente di riproduzione;
* **End Point:** punto finale del loop.

Il marker Current Point deve essere sempre visibile e deve muoversi dinamicamente durante la riproduzione.

I marker Starting Point ed End Point devono essere visualizzati quando la modalità Loop è attiva o durante la configurazione dell’intervallo di loop.

L’utente deve poter:

* trascinare i marker direttamente sulla waveform;
* modificare numericamente la loro posizione;
* effettuare zoom orizzontale;
* scorrere la waveform;
* ripristinare rapidamente il loop all’intera durata del file;
* salvare i marker nei metadati della Reference.

La zona compresa tra Starting Point ed End Point deve essere evidenziata graficamente.

La waveform deve indicare chiaramente:

* durata totale;
* tempo corrente;
* durata della selezione;
* stato del loop;
* stato di riproduzione;
* eventuale offset iniziale.

#### Layout dinamico e organizzazione dei pannelli

L’interfaccia deve essere riorganizzata in pannelli funzionali chiaramente separati.

Pannelli principali:

* Reference Library;
* Reference Player e Waveform;
* A/B e Loudness Matching;
* EQ di confronto;
* Spectrum Analyzer;
* Meter e analizzatori;
* Impostazioni.

I pannelli devono poter utilizzare, dove appropriato:

* sezioni espandibili e comprimibili;
* menu a tendina;
* tab;
* finestre ridimensionabili;
* separatori trascinabili;
* pannelli mobili o ancorabili;
* viste compatte e dettagliate.

Gli strumenti meno utilizzati non devono occupare permanentemente spazio nell’interfaccia e devono poter essere raccolti in sezioni richiudibili.

Il layout deve adattarsi automaticamente alle dimensioni della finestra del plugin, mantenendo leggibili i controlli principali.

L’utente deve poter ridimensionare la finestra e, se tecnicamente sostenibile, modificare le proporzioni tra i pannelli.

Lo stato del layout deve essere salvato e ripristinato con il plugin, includendo:

* pannelli aperti o chiusi;
* dimensioni della finestra;
* posizione dei separatori;
* tab selezionata;
* modalità compatta o completa.

#### Obiettivo dell’interfaccia

L’interfaccia della versione 2 deve consentire all’utente di identificare immediatamente:

* quale sorgente è in ascolto;
* quale sorgente appartiene a ogni misurazione;
* quale intervallo della Reference è in loop;
* quale parte dello spettro viene influenzata dall’EQ;
* quali strumenti di analisi sono attivi;
* lo stato di caricamento, sincronizzazione e riproduzione della Reference.

Il layout deve privilegiare chiarezza, coerenza visiva e rapidità di utilizzo, evitando pannelli sovraccarichi o informazioni duplicate.

- uso libreiria grafica professionale tramite la repo https://github.com/ffAudio/foleys_gui_magic
- aggiunta metere di Airwindows (file presenti nella cartella Meter-0.2.8) o alla repo https://github.com/airwindows/Meter/

## 1.5 Fuori ambito

Non fanno parte della roadmap coperta da questo documento:

- streaming Spotify;
- streaming YouTube;
- integrazione Bandcamp;
- intelligenza artificiale per il suggerimento automatico delle reference;
- match EQ automatico;
- mastering automatico;
- supporto mobile;
- formato AAX;
- piattaforme Linux.

---

# 2. Glossario

| Termine | Definizione |
|---|---|
| Mix | Segnale audio ricevuto dal plugin dalla DAW |
| Reference | Traccia audio usata come riferimento |
| A/B | Confronto alternato tra Mix e Reference |
| Loudness Matching | Adeguamento del livello percepito tra le due sorgenti |
| Integrated LUFS | Loudness medio misurato sull'intero intervallo analizzato |
| Short-Term LUFS | Loudness misurato su una finestra temporale breve |
| Momentary LUFS | Loudness misurato su una finestra molto breve |
| Peak | Picco massimo del segnale |
| True Peak | Stima dei picchi inter-sample |
| RMS | Valore quadratico medio del segnale |
| Crest Factor | Differenza tra picco e livello medio |
| Dynamic Range | Stima dell'escursione dinamica |
| Correlation | Relazione di fase tra canale sinistro e destro |
| Stereo Width | Stima normalizzata della larghezza stereo |
| Mid | Componente comune ai canali sinistro e destro |
| Side | Componente differenziale tra sinistro e destro |
| Provider | Modulo che fornisce metadati e accesso audio a una sorgente |
| Cache | Memoria temporanea usata per ridurre accessi al disco o alla rete |
| Transport | Stato di riproduzione della DAW |
| Offline Rendering | Esportazione eseguita dalla DAW in modalità non real-time |

---

# 3. Contesto operativo

## 3.1 Ambiente host

ReferenceLab viene caricato come plugin VST3 all'interno di una DAW compatibile.

Il plugin deve:

- ricevere audio stereo dal bus principale;
- accedere alle informazioni di transport quando disponibili;
- produrre un'uscita stereo;
- salvare e ripristinare lo stato tramite il sistema di preset della DAW;
- rilevare, quando possibile, il processing non real-time.

## 3.2 Piattaforme

### Versione 1

- Windows 10 64-bit;
- Windows 11 64-bit;
- formato VST3;
- processori x86-64.

### Versione 2

La versione 2 resta focalizzata su Windows e VST3. Il supporto macOS o AU potrà essere valutato in un documento successivo.

## 3.3 Dipendenze principali

- C++20;
- JUCE;
- JUCE Audio Formats;
- JUCE DSP;
- JUCE GUI;
- supporto JSON;
- sistema di threading JUCE o standard C++;
- sistema di file I/O asincrono.

---

# 4. Principi di progettazione

## 4.1 Separazione tra audio thread e UI thread

Il thread audio non deve:

- eseguire accessi al disco;
- leggere o scrivere JSON;
- effettuare allocazioni non controllate;
- aprire file;
- compiere operazioni bloccanti;
- eseguire chiamate di rete.

## 4.2 Elaborazioni identiche per il confronto

Le elaborazioni di monitoraggio devono essere applicate in modo coerente alle due sorgenti.

In particolare:

- EQ di confronto;
- Stereo;
- Mono;
- Mid;
- Side;
- Swap Left/Right;
- eventuali filtri di ascolto.

La finalità è evitare che il confronto venga alterato da processing differenti.

## 4.3 Uscita trasparente

Quando è selezionata la sorgente Mix e tutte le funzioni di confronto sono bypassate, il segnale in uscita deve risultare equivalente al segnale in ingresso, entro la tolleranza numerica del processing floating-point.

## 4.4 Architettura multi-reference

Anche se la prima interfaccia può mostrare una sola reference attiva, il modello interno deve supportare una collezione di reference.

Esempio concettuale:

```cpp
std::vector<ReferenceTrack> references;
std::optional<ReferenceId> activeReference;
```

Non deve essere usato un modello rigido basato su una sola istanza globale.

## 4.5 Architettura a provider

L'accesso alle reference deve essere astratto tramite un'interfaccia comune.

Provider previsti:

- `LocalReferenceProvider`;
- `JsonCatalogProvider`;
- `HttpReferenceProvider`;
- `CambridgeReferenceProvider`.

La versione 1 implementa il provider locale. La versione 2 aggiunge gli altri provider.

---

# 5. Architettura logica

## 5.1 Diagramma generale

```text
+---------------------------------------------------------------+
|                         Plugin Editor                         |
|                                                               |
|  Library  Player  Waveform  A/B  EQ  Analysis  Settings       |
+------------------------------+--------------------------------+
                               |
                               v
+---------------------------------------------------------------+
|                         Application Layer                     |
|                                                               |
|  ReferenceManager  StateManager  TransportController          |
|  CacheManager      MetadataRepository  UIModel                 |
+------------------------------+--------------------------------+
                               |
                               v
+---------------------------------------------------------------+
|                         Audio Layer                           |
|                                                               |
|  AudioEngine  ReferencePlayer  ComparisonProcessor            |
|  LoudnessMatcher  ABRouter  AnalysisEngine                    |
+------------------------------+--------------------------------+
                               |
                               v
+---------------------------------------------------------------+
|                         Provider Layer                        |
|                                                               |
|  IReferenceProvider                                           |
|      |- LocalReferenceProvider                                |
|      |- JsonCatalogProvider          [v2]                     |
|      |- HttpReferenceProvider         [v2]                     |
|      `- CambridgeReferenceProvider    [v2]                     |
+---------------------------------------------------------------+
```

## 5.2 Moduli principali

### 5.2.1 AudioEngine

Responsabilità:

- ricevere il segnale dalla DAW;
- ottenere il segnale della reference;
- gestire il routing A/B;
- applicare le elaborazioni di confronto;
- mantenere la continuità audio;
- gestire il rendering offline;
- fornire dati agli analizzatori.

### 5.2.2 ReferencePlayer

Responsabilità:

- riprodurre la reference attiva;
- seek;
- play;
- pause;
- stop;
- loop;
- offset iniziale;
- adattamento al sample rate host;
- gestione del buffer della reference.

### 5.2.3 ReferenceManager

Responsabilità:

- elenco reference;
- aggiunta;
- rimozione;
- aggiornamento metadati;
- ricerca;
- filtri;
- ordinamento;
- selezione reference attiva;
- coordinamento provider;
- coordinamento cache.

### 5.2.4 MetadataRepository

Responsabilità:

- caricare `reference.json`;
- validare il contenuto;
- salvare modifiche;
- creare backup;
- gestire migrazioni di schema;
- impedire corruzione del database.

### 5.2.5 CacheManager

Responsabilità:

- precaricare reference;
- mantenere in memoria le reference recenti;
- applicare una strategia LRU o equivalente;
- rispettare un limite massimo di memoria;
- invalidare la cache quando il file cambia.

### 5.2.6 ComparisonProcessor

Responsabilità:

- High Pass;
- Band Pass;
- Low Pass;
- Stereo;
- Mono;
- Mid;
- Side;
- bypass globale;
- applicazione identica alle due sorgenti.

### 5.2.7 LoudnessMatcher

Responsabilità:

- analizzare loudness della reference;
- analizzare loudness del mix;
- calcolare gain di compensazione;
- applicare gain con smoothing;
- offrire modalità automatica e manuale;
- impedire salti improvvisi di livello.

### 5.2.8 AnalysisEngine

Responsabilità:

- FFT;
- oscilloscopio;
- LUFS;
- RMS;
- Peak;
- True Peak;
- Crest Factor;
- Dynamic Range;
- Correlation;
- Stereo Width;
- condivisione thread-safe dei risultati con la GUI.

### 5.2.9 TransportController

Responsabilità:

- leggere stato play/stop;
- leggere posizione temporale;
- leggere BPM host;
- leggere time signature quando disponibile;
- sincronizzare reference e DAW;
- gestire fallback quando il transport non è disponibile.

### 5.2.10 StateManager

Responsabilità:

- serializzare lo stato del plugin;
- ripristinare parametri;
- memorizzare ultima reference;
- memorizzare loop;
- memorizzare EQ;
- memorizzare modalità di ascolto;
- separare stato globale e stato per progetto.

---

# 6. Flusso audio

## 6.1 Diagramma del segnale

```text
                         +------------------+
DAW INPUT -------------->| Mix Input Buffer |
                         +---------+--------+
                                   |
                                   v
                         +------------------+
                         | Mix Analysis Tap |
                         +---------+--------+
                                   |
                                   v
+--------------------+    +----------------------+
| Reference Provider |--->| ReferencePlayer      |
+--------------------+    +----------+-----------+
                                    |
                                    v
                           +----------------------+
                           | Reference Analysis   |
                           +----------+-----------+
                                      |
                +---------------------+---------------------+
                |                                           |
                v                                           v
       +------------------+                       +------------------+
       | Mix Comparison   |                       | Ref Comparison   |
       | Processing       |                       | Processing       |
       +--------+---------+                       +--------+---------+
                |                                           |
                +---------------------+---------------------+
                                      |
                                      v
                           +----------------------+
                           | Loudness Matching    |
                           +----------+-----------+
                                      |
                                      v
                           +----------------------+
                           | A/B Router + Fade    |
                           +----------+-----------+
                                      |
                                      v
                           +----------------------+
                           | Output Analysis Tap  |
                           +----------+-----------+
                                      |
                                      v
                                  DAW OUTPUT
```

## 6.2 Ordine delle elaborazioni

Ordine logico proposto:

1. acquisizione Mix;
2. acquisizione Reference;
3. resampling reference;
4. applicazione modalità Stereo/Mono/Mid/Side;
5. EQ di confronto;
6. gain di loudness matching;
7. A/B routing;
8. crossfade;
9. analisi del segnale in ascolto;
10. output.

L'analisi separata di Mix e Reference può essere effettuata tramite tap dedicati prima o dopo le elaborazioni di confronto, configurabili in base al parametro visualizzato.

---

# 7. Requisiti funzionali

## 7.1 Gestione audio

### RF-001 – Ingresso DAW

Il plugin deve ricevere un segnale stereo dalla DAW.

**Criterio di accettazione:** un file stereo riprodotto nella DAW attraversa il plugin senza interruzioni.

### RF-002 – Uscita stereo

Il plugin deve produrre un'uscita stereo.

### RF-003 – Bypass trasparente

Con Reference disattivata e processing di confronto bypassato, l'uscita deve essere equivalente all'ingresso.

### RF-004 – Formati reference locali

Il plugin deve supportare almeno WAV, AIFF, FLAC e MP3.

### RF-005 – Sample rate

Il plugin deve riprodurre reference con sample rate differente da quello della sessione DAW.

### RF-006 – Canali reference

Il plugin deve supportare file mono e stereo.

## 7.2 Player reference

### RF-007 – Play

L'utente deve poter avviare la riproduzione della reference.

### RF-008 – Pause

L'utente deve poter mettere in pausa la reference mantenendo la posizione corrente.

### RF-009 – Stop

L'utente deve poter arrestare la reference e riportarla al punto di partenza configurato.

### RF-010 – Seek

L'utente deve poter cambiare la posizione di riproduzione.

### RF-011 – Offset iniziale

Ogni reference deve poter avere un punto di inizio personalizzato.

### RF-012 – Memorizzazione offset

L'offset iniziale deve essere salvato nei metadati.

### RF-013 – Fine file

Al raggiungimento della fine del file, il player deve arrestarsi oppure tornare al punto A se il loop è attivo.

## 7.3 Loop

### RF-014 – Marker A e B

L'utente deve poter impostare un punto A e un punto B.

### RF-015 – Validazione loop

Il punto B deve essere successivo al punto A.

### RF-016 – Loop continuo

La reference deve riprodurre ciclicamente l'intervallo A-B.

### RF-017 – Modifica marker

I marker devono essere modificabili tramite trascinamento o inserimento numerico.

### RF-018 – Stato loop per reference

I marker devono poter essere salvati per ogni reference.

### RF-019 – Crossfade loop

Il sistema deve consentire un breve crossfade opzionale nel punto di ripetizione.

## 7.4 Confronto A/B

### RF-020 – Selezione Mix

L'utente deve poter ascoltare esclusivamente il Mix.

### RF-021 – Selezione Reference

L'utente deve poter ascoltare esclusivamente la Reference.

### RF-022 – Cambio istantaneo

Il cambio A/B deve essere percepito come immediato.

### RF-023 – Crossfade configurabile

Il cambio A/B deve supportare un crossfade configurabile tra 0 e 100 ms.

### RF-024 – Assenza di click

Il cambio A/B non deve produrre click o pop in condizioni operative normali.

### RF-025 – Controllo tramite tastiera

Il cambio A/B deve poter essere associato a una scorciatoia interna al plugin, compatibilmente con le limitazioni della DAW.

## 7.5 Loudness matching

### RF-026 – Modalità automatica

Il plugin deve offrire loudness matching automatico.

### RF-027 – Modalità Integrated

Il sistema deve poter calcolare la compensazione usando Integrated LUFS.

### RF-028 – Modalità Short-Term

Il sistema deve poter calcolare la compensazione usando Short-Term LUFS.

### RF-029 – Modalità Momentary

Il sistema deve poter calcolare la compensazione usando Momentary LUFS.

### RF-030 – Gain manuale

L'utente deve poter applicare una correzione manuale aggiuntiva.

### RF-031 – Bypass

L'utente deve poter disattivare il loudness matching.

### RF-032 – Smoothing

Le variazioni automatiche di gain devono essere smussate.

### RF-033 – Limite di sicurezza

Il gain automatico deve essere limitato a un intervallo configurabile.

## 7.6 EQ di confronto

### RF-034 – Applicazione simmetrica

L'EQ di confronto deve essere applicato con gli stessi parametri a Mix e Reference.

### RF-035 – High Pass

Il sistema deve includere un filtro High Pass con frequenza, pendenza e bypass.

### RF-036 – Band Pass

Il sistema deve includere un filtro Band Pass con frequenza centrale, gain, Q e bypass.

### RF-037 – Low Pass

Il sistema deve includere un filtro Low Pass con frequenza, pendenza e bypass.

### RF-038 – Bypass EQ

L'intero EQ deve poter essere bypassato.

### RF-039 – Scopo di monitoraggio

L'EQ non deve modificare i file sorgente.

## 7.7 Modalità di ascolto

### RF-040 – Stereo

L'utente deve poter ascoltare entrambe le sorgenti in Stereo.

### RF-041 – Mono

L'utente deve poter ascoltare entrambe le sorgenti in Mono.

### RF-042 – Mid

L'utente deve poter ascoltare esclusivamente il contenuto Mid.

### RF-043 – Side

L'utente deve poter ascoltare esclusivamente il contenuto Side.

### RF-044 – Applicazione identica

La modalità selezionata deve essere applicata in modo identico a Mix e Reference.

### RF-045 – Cambio senza click

Il cambio di modalità deve utilizzare smoothing o crossfade.

### RF-045A – Swap Left/Right

Il pannello Analysis deve includere un pulsante **Swap L/R** che, quando attivo, invia il canale destro all'uscita sinistra e il canale sinistro all'uscita destra.

Lo scambio deve essere applicato in modo identico a Mix e Reference, senza modificare i file sorgente. L'attivazione e la disattivazione devono avvenire senza click udibili, lo stato attivo deve essere chiaramente visibile e il valore deve essere salvato e ripristinato nello stato del plugin.

## 7.8 Analisi

### RF-046 – Analisi separata

Il sistema deve analizzare separatamente Mix e Reference.

### RF-047 – FFT

Il plugin deve mostrare lo spettro FFT.

### RF-048 – Overlay FFT

Il plugin deve sovrapporre gli spettri di Mix e Reference.

### RF-049 – Configurazione FFT

L'utente deve poter configurare dimensione FFT, smoothing, averaging e velocità di aggiornamento.

### RF-049A – Slope degli spettri

Il pannello Analysis deve fornire un pomello Slope configurabile da 0,0 a 4,5 dB/ottava, con valore predefinito 4,5 dB/ottava e risoluzione massima 0,1 dB/ottava.

Lo Slope deve essere una compensazione esclusivamente grafica, applicata uniformemente a tutti gli spettri FFT visualizzati. Il controllo deve rimanere operativo in modalità Freeze, deve mostrare il valore numerico corrente e deve essere salvato e ripristinato nello stato del plugin.

### RF-050 – Oscilloscopio

Il plugin deve mostrare la forma d'onda temporale.

### RF-051 – LUFS

Il plugin deve mostrare Integrated, Short-Term e Momentary LUFS.

### RF-052 – Peak

Il plugin deve mostrare il peak level.

### RF-053 – True Peak

Il plugin deve poter mostrare il True Peak.

### RF-054 – RMS

Il plugin deve mostrare il livello RMS.

### RF-055 – Crest Factor

Il plugin deve mostrare il Crest Factor.

### RF-056 – Dynamic Range

Il plugin deve mostrare un indicatore di Dynamic Range.

### RF-057 – Correlation Meter

Il plugin deve mostrare un correlation meter da -1 a +1.

### RF-058 – Stereo Width

Il plugin deve mostrare una misura normalizzata della larghezza stereo tra 0 e 1.

### RF-059 – Reset misurazioni

L'utente deve poter azzerare i valori integrati e massimi.

### RF-060 – Freeze display

L'utente deve poter congelare temporaneamente le visualizzazioni.

## 7.9 Libreria reference

### RF-061 – Cartella libreria

L'utente deve poter selezionare una cartella locale collegata al plugin.

### RF-062 – Scansione

Il plugin deve individuare i file audio supportati presenti nella cartella.

### RF-063 – Scansione ricorsiva

L'utente deve poter scegliere se includere le sottocartelle.

### RF-064 – Aggiunta manuale

L'utente deve poter aggiungere un singolo file esterno.

### RF-065 – Rimozione dalla libreria

L'utente deve poter rimuovere una voce dalla libreria senza cancellare il file audio.

### RF-066 – Eliminazione file

L'eventuale eliminazione fisica del file deve richiedere conferma esplicita.

### RF-067 – Modifica metadati

L'utente deve poter modificare titolo, artista, album, genere, anno, BPM, tonalità, rating, preferito, note e tag.

### RF-068 – Salvataggio JSON

Le modifiche devono essere salvate in `reference.json`.

### RF-069 – Ricerca testuale

La ricerca deve includere titolo, artista, album, genere, note e tag.

### RF-070 – Filtri

Il sistema deve offrire filtri per genere, artista, tonalità, intervallo BPM, preferiti e rating.

### RF-071 – Ordinamento

L'utente deve poter ordinare per titolo, artista, album, genere, anno, BPM, rating, data aggiunta e ultimo utilizzo.

### RF-072 – Reference mancanti

Se un file non è disponibile, la voce deve essere segnalata come mancante senza bloccare la libreria.

### RF-073 – Ricollegamento file

L'utente deve poter indicare un nuovo percorso per una reference mancante.

## 7.10 Cache

### RF-074 – Cache audio

Il sistema deve poter mantenere in memoria una o più reference.

### RF-075 – Limite cache

L'utente deve poter configurare un limite massimo di memoria.

### RF-076 – Strategia espulsione

Il sistema deve rimuovere automaticamente dalla cache gli elementi meno recenti quando supera il limite.

### RF-077 – Precaricamento

Il sistema deve poter precaricare la reference selezionata prima del playback.

### RF-078 – Invalidazione cache

La cache deve essere invalidata quando il file sorgente viene modificato.

### RF-079 – Stato cache

L'interfaccia deve indicare se una reference è non caricata, in caricamento, in cache o in errore.

## 7.11 Transport e sincronizzazione

### RF-080 – Stato host

Il plugin deve leggere play e stop dalla DAW quando disponibili.

### RF-081 – Posizione host

Il plugin deve leggere la posizione temporale della DAW quando disponibile.

### RF-082 – BPM host

Il plugin deve leggere il BPM della DAW quando disponibile.

### RF-083 – Modalità libera

La reference deve poter essere riprodotta indipendentemente dal transport.

### RF-084 – Modalità sync

La reference deve poter seguire il transport della DAW.

### RF-085 – Offset sincronizzazione

L'utente deve poter impostare un offset tra DAW e reference.

### RF-086 – Fallback

Se le informazioni di transport non sono disponibili, il plugin deve tornare alla modalità libera senza interrompere l'audio.

### RF-087 – Persistenza modalità

La modalità di sincronizzazione deve essere salvata nello stato del plugin.

## 7.12 Rendering offline

### RF-088 – Rilevamento non real-time

Il plugin deve rilevare, quando il formato host lo consente, il processing non real-time.

### RF-089 – Esclusione reference

Durante il rendering offline la reference non deve essere inviata all'output.

### RF-090 – Bypass confronto

Durante il rendering offline le funzioni destinate esclusivamente al monitoraggio devono essere escluse dall'output.

### RF-091 – Output del mix

Il file esportato deve contenere il Mix, non la Reference.

### RF-092 – Ripristino stato

Terminato il rendering, il plugin deve ripristinare lo stato precedente.

### RF-093 – Safe Export Mode

Il plugin deve offrire un'opzione di sicurezza che escluda sempre la reference durante il processing non real-time.

### RF-094 – Limitazioni host

Se l'host non comunica correttamente lo stato offline, il plugin deve mostrare un avviso nella documentazione e nelle impostazioni.

## 7.13 Stato e preset

### RF-095 – Salvataggio stato DAW

Il plugin deve salvare reference attiva, posizione, loop, modalità A/B, EQ, modalità di ascolto, Swap L/R, loudness matching, analisi e sync.

### RF-096 – Percorsi file

Il sistema deve supportare percorsi assoluti e valutare anche percorsi relativi.

### RF-097 – File mancante al ripristino

Se una reference non viene trovata, il progetto deve comunque aprirsi.

### RF-098 – Preset utente

L'utente deve poter salvare preset delle impostazioni di confronto.

### RF-099 – Separazione preset e libreria

I preset non devono duplicare i file audio.

## 7.14 Provider

### RF-100 – Interfaccia comune

Ogni provider deve implementare un'interfaccia comune per elenco, apertura e metadati.

### RF-101 – Provider locale

La versione 1 deve includere `LocalReferenceProvider`.

### RF-102 – Cataloghi JSON

La versione 2 deve supportare cataloghi JSON importabili.

### RF-103 – Sorgenti HTTP

La versione 2 deve supportare URL HTTP/HTTPS diretti a file audio compatibili.

### RF-104 – Cambridge MT

La versione 2 può includere un provider Cambridge MT basato su metadati e URL pubblici, subordinato a verifica legale e autorizzativa.

### RF-105 – Errori provider

Gli errori di un provider non devono bloccare gli altri provider.

---

# 8. Requisiti non funzionali

## RNF-001 – Real-time safety

Il thread audio deve essere real-time safe.

## RNF-002 – Assenza di operazioni bloccanti

Il thread audio non deve eseguire I/O su disco, rete, lock non bounded, parsing JSON o allocazioni non controllate.

## RNF-003 – Stabilità

Il plugin non deve causare crash della DAW in caso di file corrotto, JSON invalido, reference mancante, errore di decodifica o provider non disponibile.

## RNF-004 – Latenza

L'elaborazione di confronto deve aggiungere una latenza trascurabile. Qualora venga introdotta latenza, questa deve essere comunicata alla DAW.

## RNF-005 – CPU

Obiettivo iniziale: meno del 5% di CPU su un sistema di riferimento a 48 kHz e buffer 512 campioni.

## RNF-006 – Memoria

Il consumo della cache deve rispettare il limite impostato dall'utente.

## RNF-007 – Fluidità UI

Obiettivo: 60 FPS per waveform e FFT, con degrado controllato fino a 30 FPS.

## RNF-008 – Tempi di ricerca

Una ricerca su 500 reference deve completarsi in meno di 100 ms.

## RNF-009 – Caricamento asincrono

Il caricamento di un file non deve bloccare la GUI.

## RNF-010 – Persistenza sicura

Il salvataggio JSON deve usare una strategia atomica con file temporaneo e sostituzione finale.

## RNF-011 – Backup metadati

Il sistema deve mantenere almeno un backup recente di `reference.json`.

## RNF-012 – Compatibilità Unicode

Titoli, artisti, album, note e tag devono supportare Unicode.

## RNF-013 – Precisione analisi

Le metriche devono essere documentate e verificate tramite segnali di test.

## RNF-014 – Thread safety

La comunicazione tra audio thread e GUI deve usare strutture thread-safe e non bloccanti, per quanto possibile.

## RNF-015 – Scalabilità libreria

L'architettura deve poter gestire almeno 10.000 record.

## RNF-016 – Tempi di apertura progetto

Reference mancanti o remote non devono impedire l'apertura rapida del progetto.

## RNF-017 – Logging

Il plugin deve fornire logging diagnostico disattivabile, mai direttamente nel thread audio.

## RNF-018 – Accessibilità

I controlli principali devono avere etichette, tooltip, focus da tastiera quando possibile e valori numerici leggibili.

## RNF-019 – Ridimensionamento UI

La finestra del plugin deve essere ridimensionabile entro limiti definiti.

## RNF-020 – Compatibilità host

Il plugin deve essere testato almeno su tre DAW VST3 diffuse su Windows.

---

# 9. Modello dati

## 9.1 Struttura generale

```json
{
  "schemaVersion": 2,
  "libraryId": "local-main",
  "libraryName": "Main Reference Library",
  "rootPath": "D:/Audio/References",
  "references": [
    {
      "uuid": "3f98fb0e-79a6-47df-bb25-121234567890",
      "provider": "local",
      "title": "Example Track",
      "artist": "Example Artist",
      "album": "Example Album",
      "genre": "Rock",
      "year": 2026,
      "bpm": 120.0,
      "key": "C#m",
      "rating": 5,
      "favorite": true,
      "tags": ["wide guitars", "tight low end"],
      "notes": "Reference per il bilanciamento delle chitarre.",
      "src": "Example Artist/Example Track.wav",
      "pathType": "relative",
      "durationSeconds": 214.52,
      "sampleRate": 48000,
      "channels": 2,
      "dateAdded": "2026-07-16T14:30:00+02:00",
      "lastUsed": "2026-07-16T18:10:00+02:00",
      "startOffsetSeconds": 12.5,
      "loop": {
        "enabled": true,
        "startSeconds": 42.0,
        "endSeconds": 58.0,
        "crossfadeMs": 10.0
      },
      "analysis": {
        "integratedLufs": -10.8,
        "truePeakDbtp": -0.7,
        "rmsDbfs": -13.2,
        "crestFactorDb": 8.1,
        "stereoWidth": 0.72
      },
      "fileInfo": {
        "sizeBytes": 45200123,
        "lastModified": "2026-07-10T11:00:00+02:00",
        "contentHash": ""
      }
    }
  ]
}
```

## 9.2 Campi obbligatori

Ogni reference deve contenere almeno `uuid`, `provider`, `title` e `src`.

## 9.3 Valori nulli

I dati numerici sconosciuti devono usare `null`; le liste sconosciute un array vuoto; i booleani il valore predefinito `false`.

## 9.4 Versionamento schema

Il file deve includere `schemaVersion`. Ogni modifica incompatibile deve introdurre una migrazione.

---

# 10. Interfacce interne proposte

## 10.1 ReferenceMetadata

```cpp
struct ReferenceMetadata
{
    juce::Uuid uuid;
    juce::String providerId;
    juce::String title;
    juce::String artist;
    juce::String album;
    juce::String genre;
    juce::String musicalKey;
    juce::String notes;
    std::optional<int> year;
    std::optional<double> bpm;
    std::optional<int> rating;
    bool favourite = false;
    std::vector<juce::String> tags;
    juce::String source;
    double startOffsetSeconds = 0.0;
};
```

## 10.2 ReferenceTrack

```cpp
class ReferenceTrack
{
public:
    ReferenceMetadata metadata;
    ReferencePlaybackState playbackState;
    ReferenceAnalysisData analysisData;
};
```

## 10.3 ReferencePlayer

```cpp
class ReferencePlayer
{
public:
    bool prepare(const ReferenceOpenResult& source,
                 double hostSampleRate,
                 int maximumBlockSize);

    void play();
    void pause();
    void stop();
    void seek(double seconds);
    void setLoopRange(double startSeconds, double endSeconds);
    void setLoopEnabled(bool enabled);
    void process(juce::AudioBuffer<float>& destination,
                 int numberOfSamples);
};
```

---

# 11. User stories

- **US-001:** Come fonico, voglio passare rapidamente dal mio mix alla reference per rilevare differenze di bilanciamento.
- **US-002:** Come produttore, voglio confrontare Mix e Reference allo stesso loudness percepito.
- **US-003:** Come utente, voglio isolare le basse per confrontare il low-end.
- **US-004:** Come utente, voglio isolare una banda con un filtro Band Pass.
- **US-005:** Come fonico, voglio ascoltare solo il Side.
- **US-006:** Come fonico, voglio ascoltare in Mono.
- **US-007:** Come utente, voglio mettere in loop il ritornello della reference.
- **US-008:** Come utente, voglio cercare reference per genere, artista e tag.
- **US-009:** Come utente, voglio modificare i metadati direttamente nel plugin.
- **US-010:** Come utente, voglio che la reference sia esclusa automaticamente dall'esportazione.
- **US-011:** Come utente, voglio che una reference già caricata sia disponibile immediatamente.
- **US-012:** Come utente, voglio importare in futuro cataloghi JSON e sorgenti HTTP.

---

# 12. Casi d'uso

## UC-001 – Aggiungere una reference locale

1. l'utente seleziona `Aggiungi reference`;
2. il plugin apre il selettore file;
3. l'utente sceglie un file;
4. il plugin valida il formato;
5. il plugin legge i metadati disponibili;
6. il plugin crea un UUID;
7. il plugin aggiunge la voce alla libreria;
8. il plugin salva `reference.json`.

## UC-002 – Confronto A/B

1. l'utente seleziona Reference;
2. il plugin applica il crossfade;
3. viene ascoltata la Reference;
4. l'utente seleziona Mix;
5. il plugin applica il crossfade;
6. viene ascoltato il Mix.

## UC-003 – Loudness matching automatico

1. l'utente abilita Auto Match;
2. il plugin analizza le sorgenti;
3. il plugin calcola la differenza di loudness;
4. il plugin applica il gain;
5. l'interfaccia mostra il gain applicato.

## UC-004 – Loop di una sezione

1. l'utente imposta il marker A;
2. l'utente imposta il marker B;
3. l'utente abilita Loop;
4. il player ripete l'intervallo;
5. il plugin salva i marker.

## UC-005 – Rendering offline

1. l'utente avvia l'esportazione nella DAW;
2. il plugin rileva il processing non real-time;
3. la reference viene esclusa;
4. il rendering termina;
5. il plugin ripristina lo stato precedente.

---

# 13. Specifica GUI

```text
+--------------------------------------------------------------------------------+
| ReferenceLab | Library | Search | A/B | Auto Match | Settings                  |
+--------------------------+-----------------------------------------------------+
| LIBRARY                  | ACTIVE REFERENCE                                    |
| Search: ____________     | Title / Artist / Genre                              |
| Genre: [All]             | [Waveform ---------------------------------------]  |
| Artist: [All]            |        A=====================B                      |
| Favorites: [ ]           | [Play] [Stop] [Loop] [Sync] [Start Offset]          |
| Track 1                  | A/B: [MIX] [REFERENCE]   Fade: 20 ms                |
| Track 2                  | Match: [Auto] Gain: -2.4 dB                          |
+--------------------------+-----------------------------------------------------+
| COMPARISON EQ                                                                  |
| HPF: 80 Hz | Band Pass: 2.7 kHz / +0.0 dB / Q 1.0 | LPF: 12 kHz             |
| Mode: [Stereo] [Mono] [Mid] [Side]                         [Swap L/R]          |
+--------------------------------------------------------------------------------+
| FFT OVERLAY                                      Slope: [ 4.5 dB/oct ]         |
+--------------------------------------------------------------------------------+
| MIX              | REFERENCE         | OUTPUT                                  |
| LUFS / Peak / RMS | LUFS / Peak / RMS | Correlation / Width / Oscilloscope     |
+--------------------------------------------------------------------------------+
```

La GUI deve indicare chiaramente sorgente attiva, loudness matching, loop, sync, modalità di ascolto, filtri, stato cache e rendering offline.

---

# 14. Gestione errori

Il plugin deve gestire senza crash:

- file audio non valido;
- JSON corrotto;
- memoria insufficiente;
- provider non disponibile;
- file mancante;
- errore di decodifica;
- rete non disponibile nella v2.

Esempio di errore strutturato:

```cpp
enum class ReferenceError
{
    none,
    notFound,
    unsupportedFormat,
    decodeFailure,
    networkUnavailable,
    permissionDenied,
    invalidMetadata,
    unknown
};
```

---

# 15. Sicurezza e privacy

- La versione 1 non deve caricare file dell'utente su server esterni.
- La telemetria non è richiesta.
- Eventuali credenziali future non devono essere salvate in chiaro.
- I provider online devono rispettare termini d'uso, licenze e autorizzazioni.

---

# 16. Piano di test

## 16.1 Unit test

- parsing JSON;
- migrazione schema;
- ricerca;
- filtri;
- ordinamento;
- Mid/Side;
- Stereo Width;
- Correlation;
- Peak;
- RMS;
- Crest Factor;
- loop;
- cache LRU;
- validazione marker;
- percorsi relativi e assoluti.

## 16.2 Test audio

Segnali:

- sinusoide mono;
- stereo in fase;
- stereo fuori fase;
- rumore rosa;
- impulso;
- silenzio;
- sweep;
- file mono e stereo;
- sample rate differenti.

Verifiche:

- assenza di click;
- corretto A/B;
- corretto Mono/Mid/Side;
- risposta filtri;
- metering;
- rendering offline;
- ripristino stato.

## 16.3 Test integrazione DAW

DAW iniziali consigliate:

- REAPER;
- Ableton Live;
- FL Studio oppure Cubase.

## 16.4 Stress test

- 10.000 record nel JSON;
- 500 file mancanti;
- cambi A/B ripetuti;
- loop molto corto;
- file audio lungo;
- cache piena;
- apertura e chiusura ripetuta del plugin.

---

# 17. Criteri di accettazione della versione 1

La versione 1 è considerata accettabile quando:

1. il plugin viene caricato correttamente come VST3 su Windows;
2. i file locali supportati vengono aggiunti e riprodotti;
3. A/B non produce click udibili;
4. loudness matching è operativo;
5. loop A/B è operativo;
6. EQ viene applicato in modo identico alle due sorgenti;
7. Stereo, Mono, Mid e Side sono operativi;
8. FFT overlay e meter principali sono funzionanti;
9. i metadati vengono salvati e ripristinati;
10. la ricerca per genere, artista e titolo è funzionante;
11. la cache non blocca l'audio thread;
12. il rendering offline non include la reference;
13. il progetto DAW ripristina correttamente lo stato;
14. il plugin supera i test minimi sulle DAW selezionate.

---

# 18. Roadmap

## 18.1 Versione 1 – Local Reference Core

- VST3 Windows;
- libreria locale;
- WAV, AIFF, FLAC, MP3;
- `reference.json`;
- modifica metadati;
- ricerca, filtri e ordinamento;
- playback, seek e start offset;
- loop A/B;
- A/B Mix/Reference;
- loudness matching;
- EQ High Pass, Band Pass, Low Pass;
- Stereo, Mono, Mid, Side;
- FFT overlay;
- oscilloscopio;
- Peak, True Peak, RMS, LUFS, Crest Factor, Dynamic Range;
- Correlation e Stereo Width;
- cache;
- stato plugin e preset;
- rendering offline;
- Safe Export Mode;
- architettura interna multi-reference;
- `LocalReferenceProvider`.

## 18.2 Versione 2 – Extended Reference Sources

- `JsonCatalogProvider`;
- import cataloghi JSON;
- `HttpReferenceProvider`;
- URL HTTP/HTTPS diretti;
- catalogo Cambridge MT, subordinato ad autorizzazione;
- gestione di più librerie;
- playlist;
- cambio rapido tra più reference;
- waveform completa con zoom;
- sincronizzazione avanzata con transport DAW;
- cache persistente opzionale;
- gestione errori di rete;
- validazione URL;
- UI per selezionare sorgente e catalogo;
- pomello Slope 0,0–4,5 dB/ottava nel pannello Analysis;
- pulsante Swap L/R nel pannello Analysis.

---

# 19. Decisioni architetturali principali

- **ADR-001:** modello multi-reference interno fin dalla v1.
- **ADR-002:** accesso alle sorgenti tramite provider astratto.
- **ADR-003:** database JSON versionato.
- **ADR-004:** caricamento e cache fuori dal thread audio.
- **ADR-005:** processing identico su Mix e Reference.
- **ADR-006:** Safe Export Mode per escludere la reference dal rendering.

---

# 20. Questioni aperte

1. metrica esatta per Dynamic Range;
2. standard e implementazione True Peak;
3. strategia di resampling;
4. limite cache predefinito;
5. posizione di `reference.json`;
6. gestione file fuori dalla cartella principale;
7. parametri esposti all'automazione DAW;
8. host che non segnalano correttamente il rendering offline;
9. modello di sync musicale;
10. autorizzazione Cambridge MT;
11. gestione cover art;
12. supporto a metadati loudness esterni.

---

# 21. Conclusione

ReferenceLab deve essere progettato come un sistema modulare e real-time safe.

La versione 1 deve concentrarsi sulla qualità del confronto locale, sulla stabilità e sulla correttezza degli strumenti di analisi.

La versione 2 deve estendere le sorgenti senza modificare il nucleo del motore audio, sfruttando l'architettura a provider, il modello multi-reference e il database JSON versionato.
