---------------------------------------------------------
-- Zadání: IDS Projekt - SQL
-- Vytvořil: Michal Řezník - xrezni28
---------------------------------------------------------
-- Mírná aktualizace od odevzdaného ER diagramu v podobě opravení chyb na základě komentáře opravovaného
-- (Ostranění chybného vztahu "hledá", doplněn chybějící atributy "titul" a "množství")

------- CLEAR TABLES -------  
DROP MATERIALIZED VIEW myview;
SET SERVEROUTPUT ON;

DROP TABLE "Vypujceni";
DROP TABLE "Rezervace";
DROP TABLE "Akce";
DROP TABLE "Casopis";
DROP TABLE "Kniha";
DROP TABLE "Pokuta";
DROP TABLE "Objekty";
DROP TABLE "Pracovnik";
DROP TABLE "Zakaznik";
DROP TABLE "Osoba";
DROP SEQUENCE "IDs";


--------------------------------------     CREATING TABLES      ----------------------------------------

CREATE TABLE "Osoba"(
  "IDUzivatele" INT  NOT NULL PRIMARY KEY,
  "Jmeno" VARCHAR(32) NOT NULL,
  "Prijmeni" VARCHAR(32) NOT NULL,
  "DatumNarozeni" DATE NOT NULL,
  "Bydliste" VARCHAR(100) NOT NULL,
  "TelCislo" NUMBER NOT NULL,
  "Email" VARCHAR(32) NOT NULL 
);

CREATE TABLE "Zakaznik"(
  "PocetVypujcenychKnih" NUMBER ,
  "IDUzivatele" INT NOT NULL,

  CONSTRAINT "ZakaznikFK" FOREIGN KEY("IDUzivatele") REFERENCES "Osoba" ("IDUzivatele") ON DELETE CASCADE
);

CREATE TABLE "Pracovnik"(
  "Plat" NUMBER NOT NULL,
  "CisloUctu" NUMBER NOT NULL,
  "RodneCislo" NUMBER NOT NULL,
  "IDUzivatele" INT NOT NULL,

  CONSTRAINT "PracovnikFK" FOREIGN KEY("IDUzivatele") REFERENCES "Osoba" ("IDUzivatele") ON DELETE CASCADE
);

CREATE TABLE "Objekty"(
  "IDobjektu" INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
  "Jazyk" VARCHAR(20) NOT NULL,
  "DatumVydani" DATE NOT NULL,
  "Dostupnost" NUMBER,
  "Mnozstvi" NUMBER,
  "Cena" NUMBER
);

CREATE TABLE "Pokuta"(
  "IDPokuty" INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
  "Datum" DATE NOT NULL,
  "Prestupek"  VARCHAR(100) NOT NULL,
  "VyskaPokuty" NUMBER NOT NULL,
  "Pokutoval" INT NOT NULL,
  "Pokutovany" INT NOT NULL,
  "IDObjektus" INT NOT NULL,

  CONSTRAINT "PokutovalFK" FOREIGN KEY("Pokutoval") REFERENCES "Osoba" ("IDUzivatele") ON DELETE SET NULL,
  CONSTRAINT "PokutovanyFK" FOREIGN KEY("Pokutovany") REFERENCES "Osoba" ("IDUzivatele") ON DELETE SET NULL,
  CONSTRAINT "IDObjektusFK" FOREIGN KEY("IDObjektus") REFERENCES "Objekty" ("IDobjektu") ON DELETE SET NULL

);


CREATE TABLE "Kniha"(
  "IDKnihy" INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
  "Titul" VARCHAR(64),
  "Autor" VARCHAR(64),
  "Nakladatelstvi" VARCHAR(20) NOT NULL,
  "Zanr" VARCHAR(32) NOT NULL,
  "IDobjektu" INT NOT NULL,

  CONSTRAINT "ObjektKnihaFK" FOREIGN KEY("IDobjektu") REFERENCES "Objekty" ("IDobjektu") ON DELETE CASCADE
);

CREATE TABLE "Casopis"(
  "IDCasopisu" INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
  "Nazev" VARCHAR(64),
  "Zamereni" VARCHAR(64),
  "IDobjektu" iNT NOT NULL,

  CONSTRAINT "ObjektCasopisFK" FOREIGN KEY("IDobjektu") REFERENCES "Objekty" ("IDobjektu") ON DELETE CASCADE
);

CREATE TABLE "Akce" (
    "CisloAkce" INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
    "Datum" DATE NOT NULL,
    "IDobjektu" iNT NOT NULL,

    CONSTRAINT "ObsahAkceFK" FOREIGN KEY("IDobjektu") REFERENCES "Objekty" ("IDobjektu") ON DELETE SET NULL
);

CREATE TABLE "Rezervace" (
    "DatumExpirace" DATE NOT NULL,
    "Zakaznik" INT NOT NULL,
    "Potvrdil" INT NOT NULL,
    "IDakce" INT NOT NULL,

    CONSTRAINT "RezervaceFK" FOREIGN KEY("IDakce") REFERENCES "Akce" ("CisloAkce") ON DELETE CASCADE,
    CONSTRAINT "RezervujiciFK" FOREIGN KEY("Zakaznik") REFERENCES "Osoba" ("IDUzivatele") ON DELETE CASCADE,
    CONSTRAINT "PotvrdilFK" FOREIGN KEY("Potvrdil") REFERENCES "Osoba" ("IDUzivatele") ON DELETE SET NULL
);

CREATE TABLE "Vypujceni" (
    "DobaVypujceni" DATE NOT NULL,
    "Zakaznik" INT NOT NULL,
    "Zaznamenal" INT NOT NULL,
    "IDakce" INT NOT NULL,

    CONSTRAINT "IDVypujceniFK" FOREIGN KEY("IDakce") REFERENCES "Akce" ("CisloAkce") ON DELETE CASCADE,
    CONSTRAINT "ZakaznikVypFK" FOREIGN KEY("Zakaznik") REFERENCES "Osoba" ("IDUzivatele") ON DELETE CASCADE ,
    CONSTRAINT "ZaznamenalFK" FOREIGN KEY("Zaznamenal") REFERENCES "Osoba" ("IDUzivatele") ON DELETE SET NULL
);
--------------------------------------      TRIGGER     ----------------------------------------

----- Automatické generování klíče  tabulky Osoba -----
CREATE SEQUENCE "IDs";

CREATE OR REPLACE TRIGGER osobaID
  BEFORE INSERT ON "Osoba"
  FOR EACH ROW
  WHEN (NEW."IDUzivatele" IS NULL)
BEGIN
      :NEW."IDUzivatele" := "IDs".NEXTVAL;
END;
/
-------- Při ztrátě knihy odečtena z databáze---------
CREATE OR REPLACE TRIGGER ztrata 
  AFTER INSERT ON "Pokuta"
    FOR EACH ROW
    WHEN (NEW."Prestupek" = 'ztráta')
    BEGIN
        UPDATE "Objekty"
        SET "Mnozstvi" = "Mnozstvi" - 1
        WHERE "Objekty"."IDobjektu" = :NEW."IDObjektus";
    END;
/


--------------------------------------      INSERT      ----------------------------------------

INSERT INTO "Osoba" ("Jmeno", "Prijmeni", "DatumNarozeni", "Bydliste", "TelCislo", "Email")
VALUES ('Janko', 'Hrasko', TO_DATE('1980-08-15', 'yyyy/mm/dd'), 'Praha', 732465809, 'nevim@seznam.com');
INSERT INTO "Osoba" ("Jmeno", "Prijmeni", "DatumNarozeni", "Bydliste", "TelCislo", "Email")
VALUES ('Jan', 'Novák', TO_DATE('2000-03-01', 'yyyy/mm/dd'), 'Brno', 739234903, 'IDs@gmail.com');


INSERT INTO "Zakaznik" ("PocetVypujcenychKnih","IDUzivatele")
VALUES (2,1);

INSERT INTO "Pracovnik" ("Plat","RodneCislo", "CisloUctu","IDUzivatele")
VALUES (14400, 12623456, 123456489,2);


INSERT INTO "Objekty" ("Jazyk", "DatumVydani", "Dostupnost", "Mnozstvi","Cena")
VALUES ('Anglictina',TO_DATE('2040-07-06', 'yyyy/mm/dd'),5,10,1000);
INSERT INTO "Objekty" ("Jazyk", "DatumVydani", "Dostupnost", "Mnozstvi","Cena")
VALUES ('Cestina',TO_DATE('2004-10-10', 'yyyy/mm/dd'),2,3,2000);

INSERT INTO "Pokuta" ("Datum","Prestupek","VyskaPokuty","Pokutoval","Pokutovany","IDObjektus")
VALUES (TO_DATE('2000-05-06', 'yyyy/mm/dd'),'ztráta',500, 2,1,1);

INSERT INTO "Pokuta" ("Datum","Prestupek","VyskaPokuty","Pokutoval","Pokutovany","IDObjektus")
VALUES (TO_DATE('2001-05-06', 'yyyy/mm/dd'),'poškození obalu',150, 2,1,2);

INSERT INTO "Kniha"("Titul", "Autor","Nakladatelstvi","Zanr","IDobjektu")
VALUES ('Bible', NULL, 'Albatros', 'Nabozenske',1);

INSERT INTO "Casopis" ("Nazev", "Zamereni", "IDobjektu")
VALUES ('Forbes','Finance',2);

INSERT INTO "Akce"("Datum","IDobjektu")
VALUES (TO_DATE('2004-10-10', 'yyyy/mm/dd'),1);
INSERT INTO "Akce"("Datum","IDobjektu")
VALUES (TO_DATE('2004-10-10', 'yyyy/mm/dd'),2);

INSERT INTO "Rezervace"("DatumExpirace","Zakaznik","Potvrdil","IDakce")
VALUES (TO_DATE('2004-10-10', 'yyyy/mm/dd'),1,2,2);

INSERT INTO "Vypujceni"("DobaVypujceni","Zakaznik","Zaznamenal","IDakce")
VALUES (TO_DATE('2004-10-10', 'yyyy/mm/dd'),1,2,1);

--------------------------------------      PROCEDURY      ----------------------------------------
-- Provede vypsaní podílu uživatelů dané domény
CREATE OR REPLACE PROCEDURE emaily (email IN VARCHAR)
IS
  CURSOR naseemaily IS SELECT * FROM "Osoba"; 
  polozka naseemaily%ROWTYPE;              
  pocet NUMBER;                         
  celkem NUMBER;     
  domena VARCHAR(32);                   
BEGIN
  pocet := 0;                     
  celkem := 0;
  OPEN naseemaily;
  LOOP
    FETCH naseemaily INTO polozka;        
    EXIT WHEN naseemaily%NOTFOUND; 
    domena := SUBSTR(polozka."Email",INSTR(polozka."Email",'@')+1, LENGTH(polozka."Email"));
    IF (domena = email)THEN  
      pocet := pocet + 1;
    END IF;
    celkem := celkem + 1;
  END LOOP;
  DBMS_OUTPUT.PUT_LINE('Podíl emailu: ' || email || ' je ' || pocet/celkem*100 || '%.');

  EXCEPTION
    WHEN ZERO_DIVIDE THEN
      DBMS_OUTPUT.PUT_LINE('Podíl emailu: ' || email || ' je 0%.');
    WHEN OTHERS THEN
      RAISE_APPLICATION_ERROR(-20206, 'Nastala chyba pri provadeni procedury.');
END;
/
BEGIN emaily ('gmail.com'); END; -------Vypíše počet zákazníků knihovny využívající gmail
/
-------Spočítá kolik potřebujeme na výplaty za měíc pro zaměstnance------------
CREATE OR REPLACE PROCEDURE vydaje
IS
  CURSOR pracovnik IS SELECT * FROM "Pracovnik";
  polozkaa "Pracovnik"%ROWTYPE;
  plat INTEGER;
  pocet INTEGER;
BEGIN
  plat := 0;
  pocet := 0;
  OPEN pracovnik;
  LOOP
    FETCH pracovnik INTO polozkaa;
    EXIT WHEN pracovnik%NOTFOUND;
    plat := plat + polozkaa."Plat";
    pocet := pocet + 1;
  END LOOP;
  DBMS_OUTPUT.PUT_LINE('Na výplaty pro počet zaměstnanců: ' || pocet || ' bude potřeba: ' || plat ||'Kc');
  EXCEPTION
  WHEN ZERO_DIVIDE THEN
  DBMS_OUTPUT.PUT_LINE('Máme 0 zaměstnanců, neděláme výplaty');
END;
/
BEGIN vydaje; END; -------Kolik tento měsíc bude potřeba na výplaty?
/

-----------------------------------      SELECT      ----------------------------------------

----  NALEZNE POČET VYPUJČENÝCH KNIH VŠECH NOVÁKŮ  ------
SELECT "PocetVypujcenychKnih"
FROM  "Zakaznik"
INNER JOIN "Osoba"
ON  "Osoba"."Prijmeni" = 'Novák';

----  NALEZNE VŠECHNY ČASOPISY V ANGLIČTINĚ ------
SELECT "Nazev"
FROM  "Casopis"
INNER JOIN "Objekty"
ON  "Objekty"."Jazyk" = 'Anglictina';

-------- KDO POKUTOVAL A ZA CO, PODLE UŽIVATELE ----------
SELECT
  p."Prestupek",
  k."IDUzivatele"
FROM
  "Pokuta" p
  INNER JOIN "Pracovnik" k
ON k."IDUzivatele" = p."Pokutoval"
  INNER JOIN "Zakaznik" z
ON z."IDUzivatele" = 1;

-------- Vyhledá všechny pokutované a ukáže součet jejich pokut----------
SELECT 
  "Pokutovany",
  sum("VyskaPokuty") as "SoucetPokut"
FROM
  "Pokuta"
GROUP BY
  "Pokutovany";
---------- Vypíše množství knih podle jednotlivých jazyků --------------------
SELECT
  "Jazyk",
  AVG("Mnozstvi") as "Mnozstvi knih"
FROM 
  "Objekty"
GROUP BY
  "Jazyk";
------------Všechny rezervované nebo vypujčené knihy ---------------
SELECT
  k."Titul"
FROM
  "Kniha" k
WHERE EXISTS
  (
    SELECT s."IDobjektu"
    FROM "Akce" s
    WHERE  k."IDobjektu" = s."IDobjektu"
  );
-----------Koho pokutoval zaměstanec s ID =2----------------
SELECT "Prijmeni"
FROM "Osoba"
WHERE "IDUzivatele" IN
  (
    SELECT "Pokutoval"
    FROM "Pokuta"
    WHERE "Pokutoval" = 2
  );

-------------------------------- PŘÍSTUPOVÁ PRÁVA ------------------------------------
GRANT ALL ON "Vypujceni" TO xbalaz12;
GRANT ALL ON "Rezervace" TO xbalaz12;
GRANT ALL ON "Akce" TO xbalaz12;
GRANT ALL ON "Casopis" TO xbalaz12;
GRANT ALL ON "Kniha" TO xbalaz12;
GRANT ALL ON "Pokuta" TO xbalaz12;
GRANT ALL ON "Objekty" TO xbalaz12;
GRANT ALL ON "Pracovnik" TO xbalaz12;
GRANT ALL ON "Zakaznik" TO xbalaz12;
GRANT ALL ON "Osoba" TO xbalaz12;

GRANT EXECUTE ON vydaje  TO xbalaz12;
GRANT EXECUTE ON emaily TO xbalaz12;

------------------------------ PLÁN --------------------------------
------------- úklid ----------------
ALTER TABLE "Osoba"
DROP PRIMARY KEY CASCADE;
---------- bez indexu --------------

EXPLAIN PLAN FOR
SELECT "Prijmeni", COUNT(*)
FROM "Vypujceni" NATURAL JOIN "Osoba"
GROUP BY "Prijmeni";
SELECT * FROM TABLE(DBMS_XPLAN.display);


---------- s indexem--------------
CREATE INDEX myindex ON "Vypujceni"("Zakaznik");
EXPLAIN PLAN FOR
SELECT "Prijmeni", COUNT(*)
FROM "Vypujceni" NATURAL JOIN "Osoba"
GROUP BY "Prijmeni";
SELECT * FROM TABLE(DBMS_XPLAN.display);
DROP INDEX myindex;
--------------------------- MATERIALIZOVANÝ POHLED ------------------------------

CREATE MATERIALIZED VIEW LOG ON "Pokuta" WITH PRIMARY KEY, ROWID("IDObjektus") INCLUDING NEW VALUES;
CREATE MATERIALIZED VIEW myview
CACHE
-------- optimalizovat čtení z daného pohledu  
BUILD IMMEDIATE
-------- naplňování daného pohledu po vytvoření
REFRESH ON COMMIT
-------- aktualizování po commitu
ENABLE QUERY REWRITE
-------- optimalizátor
  AS SELECT "IDObjektus", COUNT(*)
  FROM "Pokuta"
  GROUP BY "IDObjektus";
    
GRANT ALL ON myview TO xbalaz12;

-------Ukázka pohledu ---------
SELECT * FROM "Pokuta";
INSERT INTO "Pokuta" ("Datum","Prestupek","VyskaPokuty","Pokutoval","Pokutovany","IDObjektus")
  VALUES(TO_DATE('2004-02-03', 'yyyy/mm/dd'),'Polití','200',2,1,2);
COMMIT;
SELECT * FROM "Pokuta";