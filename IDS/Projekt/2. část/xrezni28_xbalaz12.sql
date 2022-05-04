---------------------------------------------------------
-- Zadání: IDS Projekt - SQL
-- Vytvořil: Michal Řezník - xrezni28
--           Balážec Peter - xbalaz12
---------------------------------------------------------
-- Mírná aktualizace od odevzdaného ER diagramu v podobě opravení chyb na základě komentáře opravovaného
-- (Ostranění chybného vztahu "hledá", doplněn chybějící atributy "titul" a "množství")

------- CLEAR TABLES -------  


DROP TABLE "Vypujceni";
DROP TABLE "Rezervace";
DROP TABLE "Akce";
DROP TABLE "Casopis";
DROP TABLE "Kniha";
DROP TABLE "Objekty";
DROP TABLE "Pokuta";
DROP TABLE "Pracovnik";
DROP TABLE "Zakaznik";
DROP TABLE "Osoba";



CREATE TABLE "Osoba"(
  IDUzivatele INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
  Jmeno VARCHAR(32) NOT NULL,
  Prijmeni VARCHAR(32) NOT NULL,
  DatumNarozeni DATE NOT NULL,
  Bydliste VARCHAR(100) NOT NULL,
  TelCislo NUMBER NOT NULL,
  Email VARCHAR(32) NOT NULL 
);

CREATE TABLE "Zakaznik"(
  PocetVypujcenychKnih NUMBER ,
  IDUzivatele INT NOT NULL,

  CONSTRAINT "ZakaznikFK" FOREIGN KEY(IDUzivatele) REFERENCES "Osoba" (IDUzivatele) ON DELETE CASCADE
);

CREATE TABLE "Pracovnik"(
  CisloUctu NUMBER NOT NULL,
  RodneCislo NUMBER NOT NULL,
  IDUzivatele INT NOT NULL,

  CONSTRAINT "PracovnikFK" FOREIGN KEY(IDUzivatele) REFERENCES "Osoba" (IDUzivatele) ON DELETE CASCADE
);

CREATE TABLE "Pokuta"(
  IDPokuty INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
  Datum DATE NOT NULL,
  Prestupek  VARCHAR(100) NOT NULL,
  VyskaPokuty NUMBER NOT NULL,
  Pokutoval INT NOT NULL,
  Pokutovany INT NOT NULL,

  CONSTRAINT "PokutovalFK" FOREIGN KEY(Pokutoval) REFERENCES "Osoba" (IDUzivatele) ON DELETE SET NULL,
  CONSTRAINT "PokutovanyFK" FOREIGN KEY(Pokutovany) REFERENCES "Osoba" (IDUzivatele) ON DELETE SET NULL

);

CREATE TABLE "Objekty"(
  IDobjektu INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
  Jazyk VARCHAR(20) NOT NULL,
  DatumVydani DATE NOT NULL,
  Dostupnost NUMBER,
  Mnozstvi NUMBER
);

CREATE TABLE "Kniha"(
  Titul VARCHAR(64),
  Autor VARCHAR(64),
  Nakladatelstvi VARCHAR(20) NOT NULL,
  Zanr VARCHAR(32) NOT NULL,
  IDobjektu INT NOT NULL,

  CONSTRAINT "ObjektKnihaFK" FOREIGN KEY(IDobjektu) REFERENCES "Objekty" (IDobjektu) ON DELETE CASCADE
);

CREATE TABLE "Casopis"(
  Nazev VARCHAR(64),
  Zamereni VARCHAR(64),
  IDobjektu iNT NOT NULL,

  CONSTRAINT "ObjektCasopisFK" FOREIGN KEY(IDobjektu) REFERENCES "Objekty" (IDobjektu) ON DELETE CASCADE
);

CREATE TABLE "Akce" (
    CisloAkce INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
    Datum DATE NOT NULL,
    IDobjektu iNT NOT NULL,

    CONSTRAINT "ObsahAkceFK" FOREIGN KEY(IDobjektu) REFERENCES "Objekty" (IDobjektu) ON DELETE SET NULL
);

CREATE TABLE "Rezervace" (
    DatumExpirace DATE NOT NULL,
    Zakaznik INT NOT NULL,
    Potvrdil INT NOT NULL,
    IDakce INT NOT NULL,

    CONSTRAINT "RezervaceFK" FOREIGN KEY(IDakce) REFERENCES "Akce" (CisloAkce) ON DELETE CASCADE,
    CONSTRAINT "RezervujiciFK" FOREIGN KEY(Zakaznik) REFERENCES "Osoba" (IDUzivatele) ON DELETE CASCADE,
    CONSTRAINT "PotvrdilFK" FOREIGN KEY(Potvrdil) REFERENCES "Osoba" (IDUzivatele) ON DELETE SET NULL
);

CREATE TABLE "Vypujceni" (
    DobaVypujceni DATE NOT NULL,
    Zakaznik INT NOT NULL,
    Zaznamenal INT NOT NULL,
    IDakce INT NOT NULL,

    CONSTRAINT "IDVypujceniFK" FOREIGN KEY(IDakce) REFERENCES "Akce" (CisloAkce) ON DELETE CASCADE,
    CONSTRAINT "ZakaznikVypFK" FOREIGN KEY(Zakaznik) REFERENCES "Osoba" (IDUzivatele) ON DELETE CASCADE ,
    CONSTRAINT "ZaznamenalFK" FOREIGN KEY(Zaznamenal) REFERENCES "Osoba" (IDUzivatele) ON DELETE SET NULL
);


--------------------------------------      DEBUG      ----------------------------------------

INSERT INTO "Osoba" (Jmeno, Prijmeni, DatumNarozeni, Bydliste, TelCislo, Email)
VALUES ('Janko', 'Hrasko', TO_DATE('1980-08-15', 'yyyy/mm/dd'), 'Praha', 732465809, 'nevim@gmail.com');
INSERT INTO "Osoba" (Jmeno, Prijmeni, DatumNarozeni, Bydliste, TelCislo, Email)
VALUES ('Jan', 'Novák', TO_DATE('2000-03-01', 'yyyy/mm/dd'), 'Brno', 739234903, 'random@gmail.com');


INSERT INTO "Zakaznik" (PocetVypujcenychKnih,IDUzivatele)
VALUES (2,1);

INSERT INTO "Pracovnik" (RodneCislo, CisloUctu,IDUzivatele)
VALUES (12623456, 123456489,2);

INSERT INTO "Pokuta" (Datum,Prestupek,VyskaPokuty,Pokutoval,Pokutovany)
VALUES (TO_DATE('2000-05-06', 'yyyy/mm/dd'),'ztráta,',500, 2,1);

INSERT INTO "Objekty" (Jazyk, DatumVydani, Dostupnost, Mnozstvi)
VALUES ('Anglictina',TO_DATE('2040-07-06', 'yyyy/mm/dd'),5,10);
INSERT INTO "Objekty" (Jazyk, DatumVydani, Dostupnost, Mnozstvi)
VALUES ('Cestina',TO_DATE('2004-10-10', 'yyyy/mm/dd'),2,3);

INSERT INTO "Kniha"(Titul, Autor,Nakladatelstvi,Zanr,IDobjektu)
VALUES ('Bible', NULL, 'Albatros', 'Nabozenske',1);

INSERT INTO "Casopis" (Nazev, Zamereni, IDobjektu)
VALUES ('Forbes','Finance',2);

INSERT INTO "Akce"(Datum,IDobjektu)
VALUES (TO_DATE('2004-10-10', 'yyyy/mm/dd'),1);
INSERT INTO "Akce"(Datum,IDobjektu)
VALUES (TO_DATE('2004-10-10', 'yyyy/mm/dd'),2);

INSERT INTO "Rezervace"(DatumExpirace,Zakaznik,Potvrdil,IDakce)
VALUES (TO_DATE('2004-10-10', 'yyyy/mm/dd'),1,2,2);

INSERT INTO "Vypujceni"(DobaVypujceni,Zakaznik,Zaznamenal,IDakce)
VALUES (TO_DATE('2004-10-10', 'yyyy/mm/dd'),1,2,1);