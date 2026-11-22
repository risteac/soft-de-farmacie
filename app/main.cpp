#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sqlite3.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>


inline std::chrono::year_month_day dataCurentaSistema() {
    return std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
}

inline std::string formatData(const std::chrono::year_month_day& d) {
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(4) << static_cast<int>(d.year()) << "-"
        << std::setw(2) << static_cast<unsigned>(d.month()) << "-"
        << std::setw(2) << static_cast<unsigned>(d.day());
    return oss.str();
}

// ISO 8601
inline std::chrono::year_month_day parseData(const std::string& iso) {
    if (iso.size() != 10 || iso[4] != '-' || iso[7] != '-') {
        throw std::invalid_argument("Format data invalid (asteptat YYYY-MM-DD): " + iso);
    }
    int an = std::stoi(iso.substr(0, 4));
    unsigned luna = static_cast<unsigned>(std::stoi(iso.substr(5, 2)));
    unsigned zi = static_cast<unsigned>(std::stoi(iso.substr(8, 2)));
    std::chrono::year_month_day ymd{std::chrono::year{an} / std::chrono::month{luna} / std::chrono::day{zi}};
    if (!ymd.ok()) {
        throw std::invalid_argument("Data calendaristica inexistenta: " + iso);
    }
    return ymd;
}

//skip la whitespaces
inline std::string trim(const std::string& s) {
    std::size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }
    std::size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        --end;
    }
    return s.substr(start, end - start);
}

//self explanatory
inline std::string toUpper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    return s;
}
// iesire curata, fara crash
struct IesireDinProgram : public std::exception {
    const char* what() const noexcept override { return "Oprire solicitata."; } //ts name is frying me "what()"
};

// self explanatory
inline std::string citesteLinie(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string linie;
        if (!std::getline(std::cin, linie)) {
            throw IesireDinProgram();
        }
        linie = trim(linie);
        if (linie.empty()) {
            continue; // Ignora liniile goale din fisierele de input sau apasarile accidentale de Enter
        }
        return linie;
    }
}

// self explanatory
inline int citesteInt(const std::string& prompt, int minVal, int maxVal) {
    while (true) {
        std::string s = citesteLinie(prompt);
        try {
            std::size_t idx = 0;
            int val = std::stoi(s, &idx);
            if (idx != s.size() || val < minVal || val > maxVal) {
                std::cout << "  [!] Introdu un numar intreg intre " << minVal << " si " << maxVal << ".\n";
                continue;
            }
            return val;
        } catch (...) {
            std::cout << "  [!] Valoare numerica invalida.\n";
        }
    }
}

inline double citesteDouble(const std::string& prompt, double minVal, double maxVal) {
    while (true) {
        std::string s = citesteLinie(prompt);
        try {
            std::size_t idx = 0;
            double val = std::stod(s, &idx);
            if (idx != s.size() || val < minVal || val > maxVal) {
                std::cout << "  [!] Introdu un numar intre " << minVal << " si " << maxVal << ".\n";
                continue;
            }
            return val;
        } catch (...) {
            std::cout << "  [!] Valoare zecimala invalida.\n";
        }
    }
}

inline bool citesteBool(const std::string& prompt) {
    while (true) {
        std::string s = toUpper(citesteLinie(prompt));
        if (s == "1" || s == "DA" || s == "D" || s == "Y" || s == "YES" || s == "TRUE") return true;
        if (s == "0" || s == "NU" || s == "N" || s == "NO" || s == "FALSE") return false;
        std::cout << "  [!] Raspunde cu 1/0 sau DA/NU.\n";
    }
}

inline std::chrono::year_month_day citesteData(const std::string& prompt) {
    while (true) {
        std::string s = citesteLinie(prompt);
        try {
            return parseData(s);
        } catch (const std::exception& e) {
            std::cout << "  [!] " << e.what() << "\n";
        }
    }
}

// medicament, enum + clasa

enum class TipMedicament {
    COMPRIMATE = 0,
    TABLETE,
    CAPSULE,
    PULBERI,
    SIROP,
    SUSPENSIE,
    SOLUTIE_ORALA,
    PICATURI,
    INJECTABIL_INTRAVENOS,
    INJECTABIL_INTRAMUSCULAR,
    INJECTABIL_SUBCUTANAT,
    CREMA,
    UNGUENT,
    GEL,
    PLASTURE,
    SUPOZITOARE,
    OVUL
};

inline std::string tipMedicamentToString(TipMedicament tip) {
    switch (tip) {
        case TipMedicament::COMPRIMATE: return "COMPRIMATE";
        case TipMedicament::TABLETE: return "TABLETE";
        case TipMedicament::CAPSULE: return "CAPSULE";
        case TipMedicament::PULBERI: return "PULBERI";
        case TipMedicament::SIROP: return "SIROP";
        case TipMedicament::SUSPENSIE: return "SUSPENSIE";
        case TipMedicament::SOLUTIE_ORALA: return "SOLUTIE_ORALA";
        case TipMedicament::PICATURI: return "PICATURI";
        case TipMedicament::INJECTABIL_INTRAVENOS: return "INJECTABIL_INTRAVENOS";
        case TipMedicament::INJECTABIL_INTRAMUSCULAR: return "INJECTABIL_INTRAMUSCULAR";
        case TipMedicament::INJECTABIL_SUBCUTANAT: return "INJECTABIL_SUBCUTANAT";
        case TipMedicament::CREMA: return "CREMA";
        case TipMedicament::UNGUENT: return "UNGUENT";
        case TipMedicament::GEL: return "GEL";
        case TipMedicament::PLASTURE: return "PLASTURE";
        case TipMedicament::SUPOZITOARE: return "SUPOZITOARE";
        case TipMedicament::OVUL: return "OVUL";
        default: return "NECUNOSCUT";
    }
}

inline TipMedicament citesteTipMedicament(const std::string& prompt) {
    while (true) {
        std::string s = toUpper(citesteLinie(prompt));
        try {
            std::size_t idx = 0;
            int cod = std::stoi(s, &idx);
            if (idx == s.size() &&
                cod >= static_cast<int>(TipMedicament::COMPRIMATE) &&
                cod <= static_cast<int>(TipMedicament::OVUL)) {
                return static_cast<TipMedicament>(cod);
            }
        } catch (...) {
            // Poate fi introdus sub forma de text (ex: "COMPRIMATE", "SIROP")
        }

        for (int i = static_cast<int>(TipMedicament::COMPRIMATE);
             i <= static_cast<int>(TipMedicament::OVUL); ++i) {
            TipMedicament candidat = static_cast<TipMedicament>(i);
            if (s == tipMedicamentToString(candidat)) {
                return candidat;
            }
        }
        std::cout << "  [!] Tip invalid. Introdu un cod (0-16) sau numele tipului (ex: COMPRIMATE, SIROP).\n";
    }
}

class Medicament {
private:
    double pret;
    std::string nume;
    TipMedicament tip;
    bool necesitaRetetaMedicala;
    double acoperireDeCnas;

public:
    static TipMedicament verif(int n) {
        if (static_cast<int>(TipMedicament::COMPRIMATE) > n ||
            static_cast<int>(TipMedicament::OVUL) < n) {
            throw std::out_of_range("Tip de medicament invalid!");
        }
        return static_cast<TipMedicament>(n);
    }

    Medicament(double pret, const std::string& nume, TipMedicament tip,
               bool necesitaRetetaMedicala = true, double acoperireDeCnas = 0.0)
        : pret(pret),
          nume(nume),
          tip(tip),
          necesitaRetetaMedicala(necesitaRetetaMedicala),
          acoperireDeCnas(acoperireDeCnas) {
        if (pret < 0.0) {
            throw std::invalid_argument("Pretul nu poate fi o valoare negativa!");
        }
        if (nume.empty()) {
            throw std::invalid_argument("Numele medicamentului nu poate fi gol!");
        }
        if (acoperireDeCnas < 0.0 || acoperireDeCnas > 100.0) {
            throw std::invalid_argument("Acoperirea CNAS trebuie sa fie intre 0% si 100%!");
        }
    }

    Medicament(double pret, const std::string& nume, int tip,
               bool necesitaRetetaMedicala = true, double acoperireDeCnas = 0.0)
        : Medicament(pret, nume, verif(tip), necesitaRetetaMedicala, acoperireDeCnas) {}

    double getPret() const { return pret; }
    const std::string& getNume() const { return nume; }
    TipMedicament getTipMedicament() const { return tip; }
    bool getNecesitaRetetaMedicala() const { return necesitaRetetaMedicala; }
    double getAcoperireDeCnas() const { return acoperireDeCnas; }

    /*void aplicaReducere(double reducere) {
        if (reducere < 0.0 || reducere > 100.0) {
            throw std::invalid_argument("Reducerea trebuie sa fie intre 0% si 100%!");
        }
        pret *= (1.0 - reducere / 100.0);
    } momentan neutilizat*/

    /*void aplicaAcoperireCnas() {
        pret *= (1.0 - acoperireDeCnas / 100.0);
    } momentan neutilizat*/

    double calculeazaPretFinal(bool esteAsiguratCnas, double reducereProcent = 0.0) const {
        if (reducereProcent < 0.0 || reducereProcent > 100.0) {
            throw std::invalid_argument("Reducerea trebuie sa fie intre 0% si 100%!");
        }
        double pretFinal = pret;
        if (esteAsiguratCnas && acoperireDeCnas > 0.0) {
            pretFinal *= (1.0 - acoperireDeCnas / 100.0);
        }
        if (reducereProcent > 0.0) {
            pretFinal *= (1.0 - reducereProcent / 100.0);
        }
        return std::round(pretFinal * 100.0) / 100.0;
    }

    /*bool operator==(const Medicament& m) const {
        return nume == m.nume && tip == m.tip;
    } momentan neutilizat*/

    friend std::ostream& operator<<(std::ostream& os, const Medicament& m) {
        os << m.nume << " [" << tipMedicamentToString(m.tip) << "] | "
           << std::fixed << std::setprecision(2) << m.pret << " RON"
           << " | CNAS: " << m.acoperireDeCnas << "%"
           << " | Reteta: " << (m.necesitaRetetaMedicala ? "Da" : "Nu");
        return os;
    }
};


// Lot
class Lot {
private:
    std::string numarLot;
    int cantitate;
    std::chrono::year_month_day dataExpirare;
    Medicament medicament;
    bool retras;

public:
    Lot(const std::string& numarLot, int cantitate, std::chrono::year_month_day dataExpirare,
        const Medicament& medicament, bool retras = false)
        : numarLot(numarLot),
          cantitate(cantitate),
          dataExpirare(dataExpirare),
          medicament(medicament),
          retras(retras) {
        if (numarLot.empty()) {
            throw std::invalid_argument("Numarul lotului nu poate fi gol!");
        }
        if (cantitate < 0) {
            throw std::invalid_argument("Cantitatea trebuie sa fie pozitiva sau zero!");
        }
    }

    /*void adaugaStoc(int cantitateAdaugata) {
        if (cantitateAdaugata <= 0) {
            throw std::invalid_argument("Cantitatea adaugata trebuie sa fie strict pozitiva!");
        }
        this->cantitate += cantitateAdaugata;
    } momentan neutilizata */

    /*void retrageStoc(int cantitateRetrasa) {
        if (cantitateRetrasa <= 0) {
            throw std::invalid_argument("Cantitatea retrasa trebuie sa fie strict pozitiva!");
        }
        if (cantitateRetrasa > this->cantitate) {
            throw std::runtime_error("Stoc insuficient in lotul " + numarLot + "!");
        }
        this->cantitate -= cantitateRetrasa;
    } momentan neutilizat*/

    /*void retrageLot() { this->retras = true; }
    bool esteStocEpuizat() const { return cantitate == 0; } momentan neutilizat*/

    bool esteExpirat(const std::chrono::year_month_day& dataCurenta = dataCurentaSistema()) const {
        return this->dataExpirare < dataCurenta;
    }

    /*bool expiraInUrmatoarele(int zile,
                             const std::chrono::year_month_day& dataCurenta = dataCurentaSistema()) const {
        if (zile < 0) {
            throw std::invalid_argument("Numarul de zile trebuie sa fie pozitiv!");
        }
        std::chrono::sys_days azi = std::chrono::sys_days{dataCurenta};
        std::chrono::sys_days limita = azi + std::chrono::days{zile};
        std::chrono::sys_days expirare = std::chrono::sys_days{this->dataExpirare};
        return expirare >= azi && expirare <= limita;
    }momentan neutilizat */

    /*bool poateFiVandut(const std::chrono::year_month_day& dataCurenta = dataCurentaSistema()) const {
        return !esteExpirat(dataCurenta) && !retras && cantitate > 0;
    } momentan neutilizat */

    /*bool esteDisponibil(int cantitateCeruta,
                        const std::chrono::year_month_day& dataCurenta = dataCurentaSistema()) const {
        if (cantitateCeruta <= 0) {
            throw std::invalid_argument("Cantitatea verificata trebuie sa fie strict pozitiva!");
        }
        return poateFiVandut(dataCurenta) && (this->cantitate >= cantitateCeruta);
    } momentan neutilizat*/

    const std::string& getNumarLot() const { return numarLot; }
    int getCantitate() const { return cantitate; }
    std::chrono::year_month_day getDataExpirare() const { return dataExpirare; }
    const Medicament& getMedicament() const { return medicament; }
    bool esteRetras() const { return retras; }

    /*bool operator==(const Lot& l2) const { return numarLot == l2.numarLot; } momentan neutilizat*/

    friend std::ostream& operator<<(std::ostream& os, const Lot& l) {
        os << "Lot #" << l.numarLot << " | " << l.medicament.getNume()
           << " (" << tipMedicamentToString(l.medicament.getTipMedicament()) << ")"
           << " | Cantitate: " << l.cantitate
           << " | Expira: " << formatData(l.dataExpirare)
           << " | Status: " << (l.retras ? "RETRAS" : (l.esteExpirat() ? "EXPIRAT" : "Activ"));
        return os;
    }
};

// sqlit3 wrapper

class StatementSQLite {
private:
    sqlite3_stmt* stmt = nullptr;

public:
    StatementSQLite(sqlite3* db, const std::string& sql) {
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)/*baza de date, statement, max length, statement handle (out), unused (out)*/ != SQLITE_OK) {
            throw std::runtime_error("Eroare prepare SQLite: " + std::string(sqlite3_errmsg(db)));
        }
    }

    ~StatementSQLite() {
        if (stmt) sqlite3_finalize(stmt); //daca distrugem, ne asiguram ca stergem si statementul
    }

    StatementSQLite(const StatementSQLite&) = delete; //asan imi forteaza mana
    StatementSQLite& operator=(const StatementSQLite&) = delete;

    //bind e super tare, fara sql injection    
    void bindInt(int idx, int val) {
        if (sqlite3_bind_int(stmt, idx, val) != SQLITE_OK) {
            throw std::runtime_error("Eroare bindInt SQLite!");
        }
    }

    void bindDouble(int idx, double val) {
        if (sqlite3_bind_double(stmt, idx, val) != SQLITE_OK) {
            throw std::runtime_error("Eroare bindDouble SQLite!");
        }
    }

    void bindText(int idx, const std::string& val) {
        if (sqlite3_bind_text(stmt, idx, val.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
            throw std::runtime_error("Eroare bindText SQLite!");
        }
    }

    int step() {
        int rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
            throw std::runtime_error("Eroare step SQLite: " +
                                     std::string(sqlite3_errmsg(sqlite3_db_handle(stmt))));
        }
        return rc;
    }

    void exec() {
        if (step() != SQLITE_DONE) {
            throw std::runtime_error("Eroare executie statement SQLite!");
        }
    }

    int colInt(int col) const { return sqlite3_column_int(stmt, col); }
    double colDouble(int col) const { return sqlite3_column_double(stmt, col); }
    std::string colText(int col) const {
        const unsigned char* txt = sqlite3_column_text(stmt, col);
        return txt ? reinterpret_cast<const char*>(txt) : "";
    }
};

struct MedicamentCatalog {
    int id;
    Medicament medicament;

    friend std::ostream& operator<<(std::ostream& os, const MedicamentCatalog& mc) {
        os << "[" << mc.id << "] " << mc.medicament;
        return os;
    }
};

class BazaDateSQLite {
private:
    sqlite3* db = nullptr;
    std::string caleFisier;

    void initializeazaSchema() {
        executaSQL("PRAGMA foreign_keys = ON;");

        executaSQL(R"SQL(
            CREATE TABLE IF NOT EXISTS medicamente (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                nume TEXT NOT NULL,
                tip INTEGER NOT NULL,
                pret REAL NOT NULL CHECK(pret >= 0),
                necesita_reteta INTEGER NOT NULL CHECK(necesita_reteta IN (0, 1)),
                acoperire_cnas REAL NOT NULL CHECK(acoperire_cnas >= 0 AND acoperire_cnas <= 100),
                UNIQUE(nume, tip)
            );

            CREATE TABLE IF NOT EXISTS loturi (
                numar_lot TEXT PRIMARY KEY,
                medicament_id INTEGER NOT NULL,
                cantitate INTEGER NOT NULL CHECK(cantitate >= 0),
                data_expirare TEXT NOT NULL,
                retras INTEGER NOT NULL CHECK(retras IN (0, 1)),
                FOREIGN KEY(medicament_id) REFERENCES medicamente(id) ON DELETE CASCADE
            );

            CREATE TABLE IF NOT EXISTS clienti (
                id_client TEXT PRIMARY KEY,
                nume TEXT NOT NULL,
                este_asigurat_cnas INTEGER NOT NULL CHECK(este_asigurat_cnas IN (0, 1)),
                are_reteta_valida INTEGER NOT NULL CHECK(are_reteta_valida IN (0, 1)),
                reducere_fidelitate REAL NOT NULL CHECK(reducere_fidelitate >= 0 AND reducere_fidelitate <= 100),
                buget REAL NOT NULL CHECK(buget >= 0)
            );

            CREATE TABLE IF NOT EXISTS cos_cumparaturi (
                client_id TEXT NOT NULL,
                medicament_id INTEGER NOT NULL,
                cantitate INTEGER NOT NULL CHECK(cantitate > 0),
                PRIMARY KEY(client_id, medicament_id),
                FOREIGN KEY(client_id) REFERENCES clienti(id_client) ON DELETE CASCADE,
                FOREIGN KEY(medicament_id) REFERENCES medicamente(id) ON DELETE CASCADE
            );

            CREATE TABLE IF NOT EXISTS bonuri_fiscale (
                numar_bon INTEGER PRIMARY KEY AUTOINCREMENT,
                client_id TEXT NOT NULL,
                nume_client TEXT NOT NULL,
                data_emitere TEXT NOT NULL,
                total_brut REAL NOT NULL,
                total_economisit REAL NOT NULL,
                total_de_plata REAL NOT NULL,
                FOREIGN KEY(client_id) REFERENCES clienti(id_client)
            );

            CREATE TABLE IF NOT EXISTS linii_bon (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                numar_bon INTEGER NOT NULL,
                nume_medicament TEXT NOT NULL,
                tip INTEGER NOT NULL,
                cantitate INTEGER NOT NULL,
                pret_catalog REAL NOT NULL,
                pret_final REAL NOT NULL,
                subtotal REAL NOT NULL,
                FOREIGN KEY(numar_bon) REFERENCES bonuri_fiscale(numar_bon) ON DELETE CASCADE
            );
        )SQL");
    }

public:
    explicit BazaDateSQLite(const std::string& caleFisier) : caleFisier(caleFisier) {
        if (caleFisier.empty()) {
            throw std::invalid_argument("Calea bazei de date nu poate fi goala!");
        }
        if (sqlite3_open(caleFisier.c_str(), &db) != SQLITE_OK) {
            std::string err = db ? sqlite3_errmsg(db) : "eroare necunoscuta";
            if (db) sqlite3_close(db);
            throw std::runtime_error("Nu s-a putut deschide baza de date SQLite: " + err);
        }
        initializeazaSchema();
    }

    ~BazaDateSQLite() {
        if (db) {
            sqlite3_close(db);
            db = nullptr;
        }
    }

    BazaDateSQLite(const BazaDateSQLite&) = delete; //asan, de ce ai vrea sa copiezi pointerul unei baze de date oricum???
    BazaDateSQLite& operator=(const BazaDateSQLite&) = delete;

    const std::string& getCaleFisier() const { return caleFisier; } //self-explanatory

    // functii simple, care nu returneaza
    void executaSQL(const std::string& sql) {
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::string err = errMsg ? errMsg : "eroare SQL";
            sqlite3_free(errMsg);
            throw std::runtime_error("Eroare SQLite exec: " + err);
        }
    }

    StatementSQLite pregateste(const std::string& sql) const {
        return StatementSQLite(db, sql);
    } //vezi mai jos

    long long ultimulIdInserat() const {
        return sqlite3_last_insert_rowid(db); //mai simplu de apelat
    }

    int randuriAfectate() const {
        return sqlite3_changes(db); //self-explanatory
    }

    int asiguraMedicament(const Medicament& med) {
        {
            auto st = pregateste(R"SQL(
                INSERT INTO medicamente (nume, tip, pret, necesita_reteta, acoperire_cnas)
                VALUES (?, ?, ?, ?, ?)
                ON CONFLICT(nume, tip) DO UPDATE SET
                    pret = excluded.pret,
                    necesita_reteta = excluded.necesita_reteta,
                    acoperire_cnas = excluded.acoperire_cnas;
            )SQL");
            st.bindText(1, med.getNume());
            st.bindInt(2, static_cast<int>(med.getTipMedicament()));
            st.bindDouble(3, med.getPret());
            st.bindInt(4, med.getNecesitaRetetaMedicala() ? 1 : 0);
            st.bindDouble(5, med.getAcoperireDeCnas());
            st.exec();
        }

        auto sel = pregateste("SELECT id FROM medicamente WHERE nume = ? AND tip = ?;");
        sel.bindText(1, med.getNume());
        sel.bindInt(2, static_cast<int>(med.getTipMedicament()));
        if (sel.step() == SQLITE_ROW) {
            return sel.colInt(0);
        }
        throw std::runtime_error("Nu s-a putut obtine ID-ul medicamentului din SQLite!");
    }

    std::vector<MedicamentCatalog> getToateMedicamentele() const {
        std::vector<MedicamentCatalog> rez;
        auto st = pregateste(
            "SELECT id, pret, nume, tip, necesita_reteta, acoperire_cnas "
            "FROM medicamente ORDER BY id ASC;");
        while (st.step() == SQLITE_ROW) {
            Medicament m(st.colDouble(1), st.colText(2), st.colInt(3),
                         st.colInt(4) != 0, st.colDouble(5));
            rez.push_back(MedicamentCatalog{st.colInt(0), m});
        }
        return rez;
    }

    Medicament cautaMedicamentDupaId(int id) const {
        auto st = pregateste(
            "SELECT pret, nume, tip, necesita_reteta, acoperire_cnas "
            "FROM medicamente WHERE id = ?;");
        st.bindInt(1, id);
        if (st.step() == SQLITE_ROW) {
            return Medicament(st.colDouble(0), st.colText(1), st.colInt(2),
                              st.colInt(3) != 0, st.colDouble(4));
        }
        throw std::invalid_argument("Nu exista niciun medicament cu ID-ul " + std::to_string(id) + "!");
    }

    friend std::ostream& operator<<(std::ostream& os, const BazaDateSQLite& bd) {
        os << "BazaDateSQLite [fisier: " << bd.caleFisier << ", status: conectat]";
        return os;
    }
};

class TranzactieSQLite {
private:
    BazaDateSQLite& db;
    bool finalizata = false;

public:
    explicit TranzactieSQLite(BazaDateSQLite& db) : db(db) {
        db.executaSQL("BEGIN IMMEDIATE TRANSACTION;");
    }

    void commit() {
        db.executaSQL("COMMIT;");
        finalizata = true;
    }

    ~TranzactieSQLite() {
        if (!finalizata) {
            try {
                db.executaSQL("ROLLBACK;");
            } catch (...) {}
        }
    }

    TranzactieSQLite(const TranzactieSQLite&) = delete; //e stare temporara, de ce ai vrea sa copiezi asa ceva??
    TranzactieSQLite& operator=(const TranzactieSQLite&) = delete;
};

// inventar

class Inventar {
private:
    BazaDateSQLite& db;

public:
    explicit Inventar(BazaDateSQLite& db) : db(db) {}

    void adaugaLot(const Lot& lot) {
        int medId = db.asiguraMedicament(lot.getMedicament());
        auto st = db.pregateste(R"SQL(
            INSERT INTO loturi (numar_lot, medicament_id, cantitate, data_expirare, retras)
            VALUES (?, ?, ?, ?, ?)
            ON CONFLICT(numar_lot) DO UPDATE SET
                cantitate = cantitate + excluded.cantitate,
                retras = excluded.retras;
        )SQL");
        st.bindText(1, lot.getNumarLot());
        st.bindInt(2, medId);
        st.bindInt(3, lot.getCantitate());
        st.bindText(4, formatData(lot.getDataExpirare()));
        st.bindInt(5, lot.esteRetras() ? 1 : 0);
        st.exec();
    }

    void retrageLot(const std::string& numarLot) {
        auto st = db.pregateste("UPDATE loturi SET retras = 1 WHERE numar_lot = ?;");
        st.bindText(1, numarLot);
        st.exec();
        if (db.randuriAfectate() == 0) {
            throw std::invalid_argument("Lotul '" + numarLot + "' nu a fost gasit in inventar!");
        }
    }

    void stergeLot(const std::string& numarLot) {
        auto st = db.pregateste("DELETE FROM loturi WHERE numar_lot = ?;");
        st.bindText(1, numarLot);
        st.exec();
        if (db.randuriAfectate() == 0) {
            throw std::invalid_argument("Lotul '" + numarLot + "' nu exista in inventar!");
        }
    }

    int getStocDisponibil(const Medicament& med,
                          const std::chrono::year_month_day& data = dataCurentaSistema()) const {
        auto st = db.pregateste(R"SQL(
            SELECT COALESCE(SUM(l.cantitate), 0) /* returneaza 0 in loc de null */
            FROM loturi l
            JOIN medicamente m ON l.medicament_id = m.id
            WHERE m.nume = ? AND m.tip = ? AND l.retras = 0 AND l.cantitate > 0 AND l.data_expirare >= ?;
        )SQL");
        st.bindText(1, med.getNume());
        st.bindInt(2, static_cast<int>(med.getTipMedicament()));
        st.bindText(3, formatData(data));
        return (st.step() == SQLITE_ROW) ? st.colInt(0) : 0;
    }

    bool areStocDisponibil(const Medicament& med, int cantitate) const {
        return getStocDisponibil(med) >= cantitate;
    }

    std::vector<Lot> getStocuriAproapeExpirate(
        int zile,
        const std::chrono::year_month_day& data = dataCurentaSistema()) const {
        std::chrono::year_month_day dataLimita{std::chrono::sys_days{data} + std::chrono::days{zile}};
        std::vector<Lot> rasp;

        auto st = db.pregateste(R"SQL(
            SELECT l.numar_lot, l.cantitate, l.data_expirare, l.retras,
                   m.pret, m.nume, m.tip, m.necesita_reteta, m.acoperire_cnas
            FROM loturi l
            JOIN medicamente m ON l.medicament_id = m.id
            WHERE l.retras = 0 AND l.cantitate > 0
              AND l.data_expirare >= ? AND l.data_expirare <= ?
            ORDER BY l.data_expirare ASC;
        )SQL");
        st.bindText(1, formatData(data));
        st.bindText(2, formatData(dataLimita));

        while (st.step() == SQLITE_ROW) {
            Medicament med(st.colDouble(4), st.colText(5), st.colInt(6),
                           st.colInt(7) != 0, st.colDouble(8));
            rasp.emplace_back(st.colText(0), st.colInt(1), parseData(st.colText(2)),
                              med, st.colInt(3) != 0);
        }
        return rasp;
    }

    double getValoareStocDisponibil(const std::chrono::year_month_day& data = dataCurentaSistema()) const {
        auto st = db.pregateste(R"SQL(
            SELECT COALESCE(SUM(l.cantitate * m.pret), 0.0)
            FROM loturi l
            JOIN medicamente m ON l.medicament_id = m.id
            WHERE l.retras = 0 AND l.cantitate > 0 AND l.data_expirare >= ?;
        )SQL");
        st.bindText(1, formatData(data));
        return (st.step() == SQLITE_ROW) ? st.colDouble(0) : 0.0;
    }

    int eliminaLoturiEpuizate() {
        db.executaSQL("DELETE FROM loturi WHERE cantitate <= 0;");
        return db.randuriAfectate();
    }

    void consumaStoc(const Medicament& med, int cantitateCeruta) {
        if (cantitateCeruta <= 0) {
            throw std::invalid_argument("Cantitatea de consumat trebuie sa fie pozitiva!");
        }
        if (!areStocDisponibil(med, cantitateCeruta)) {
            throw std::runtime_error("Stoc insuficient in SQLite pentru medicamentul: " + med.getNume());
        }

        auto sel = db.pregateste(R"SQL(
            SELECT l.numar_lot, l.cantitate
            FROM loturi l
            JOIN medicamente m ON l.medicament_id = m.id
            WHERE m.nume = ? AND m.tip = ?
              AND l.retras = 0 AND l.cantitate > 0 AND l.data_expirare >= ?
            ORDER BY l.data_expirare ASC;
        )SQL");
        sel.bindText(1, med.getNume());
        sel.bindInt(2, static_cast<int>(med.getTipMedicament()));
        sel.bindText(3, formatData(dataCurentaSistema()));

        int ramasDeScazut = cantitateCeruta;
        while (ramasDeScazut > 0 && sel.step() == SQLITE_ROW) {
            std::string numarLot = sel.colText(0);
            int cantitateLot = sel.colInt(1);
            int deRetras = std::min(ramasDeScazut, cantitateLot);

            auto upd = db.pregateste("UPDATE loturi SET cantitate = cantitate - ? WHERE numar_lot = ?;");
            upd.bindInt(1, deRetras);
            upd.bindText(2, numarLot);
            upd.exec();

            ramasDeScazut -= deRetras;
        }
    }

    std::vector<Lot> getToateLoturile() const {
        std::vector<Lot> rez;
        auto st = db.pregateste(R"SQL(
            SELECT l.numar_lot, l.cantitate, l.data_expirare, l.retras,
                   m.pret, m.nume, m.tip, m.necesita_reteta, m.acoperire_cnas
            FROM loturi l
            JOIN medicamente m ON l.medicament_id = m.id
            ORDER BY m.nume ASC, l.data_expirare ASC;
        )SQL");
        while (st.step() == SQLITE_ROW) {
            Medicament med(st.colDouble(4), st.colText(5), st.colInt(6),
                           st.colInt(7) != 0, st.colDouble(8));
            rez.emplace_back(st.colText(0), st.colInt(1), parseData(st.colText(2)),
                             med, st.colInt(3) != 0);
        }
        return rez;
    }

    friend std::ostream& operator<<(std::ostream& os, const Inventar& inv) {
        std::vector<Lot> loturi = inv.getToateLoturile();
        os << "=== INVENTAR SQLITE (" << loturi.size() << " loturi) ===\n";
        if (loturi.empty()) {
            os << "  [Inventar gol]\n";
        } else {
            for (const auto& lot : loturi) {
                os << "  - " << lot << "\n";
            }
        }
        os << "Valoare stoc vandabil: " << std::fixed << std::setprecision(2)
           << inv.getValoareStocDisponibil() << " RON";
        return os;
    }
};

// cos + client

struct ItemCos {
    Medicament medicament;
    int cantitate;

    friend std::ostream& operator<<(std::ostream& os, const ItemCos& item) {
        os << item.medicament.getNume()
           << " (" << tipMedicamentToString(item.medicament.getTipMedicament()) << ")"
           << " x " << item.cantitate
           << " buc. @ " << std::fixed << std::setprecision(2) << item.medicament.getPret() << " RON";
        return os;
    }
};


// TODO: are sens o implementare cu sqlite3 a cosului de cumparaturi? sau e overkill? e mai mult helper.
class CosCumparaturi {
private:
    std::vector<ItemCos> produse;

public:
    CosCumparaturi() = default;
    explicit CosCumparaturi(std::vector<ItemCos> produse) : produse(std::move(produse)) {}

    bool esteGol() const { return produse.empty(); }
    const std::vector<ItemCos>& getProduse() const { return produse; }

    /*double calculeazaTotalBrut() const {
        double total = 0.0;
        for (const auto& item : produse) total += item.medicament.getPret() * item.cantitate;
        return total;
    } momentan neutilizat */

    double calculeazaTotalFinal(bool esteAsiguratCnas, double reducereFidelitate = 0.0) const {
        double total = 0.0;
        for (const auto& item : produse) {
            total += item.medicament.calculeazaPretFinal(esteAsiguratCnas, reducereFidelitate) * item.cantitate;
        }
        return std::round(total * 100.0) / 100.0;
    }

    friend std::ostream& operator<<(std::ostream& os, const CosCumparaturi& cos) {
        if (cos.esteGol()) {
            os << "  [Cos gol]";
            return os;
        }
        for (const auto& item : cos.produse) {
            os << "  * " << item << "\n";
        }
        return os;
    }
};

class Client {
private:
    BazaDateSQLite& db;
    std::string idClient;
    std::string nume;
    bool esteAsiguratCnas;
    bool areRetetaValida;
    double reducereFidelitate;
    double buget;

    Client(BazaDateSQLite& db, std::string idClient, std::string nume,
           bool esteAsiguratCnas, bool areRetetaValida, double reducereFidelitate, double buget)
        : db(db),
          idClient(std::move(idClient)),
          nume(std::move(nume)),
          esteAsiguratCnas(esteAsiguratCnas),
          areRetetaValida(areRetetaValida),
          reducereFidelitate(reducereFidelitate),
          buget(buget) {}

    void salveazaInDB() {
        auto st = db.pregateste(R"SQL(
            INSERT INTO clienti (id_client, nume, este_asigurat_cnas, are_reteta_valida, reducere_fidelitate, buget)
            VALUES (?, ?, ?, ?, ?, ?)
            ON CONFLICT(id_client) DO UPDATE SET
                nume = excluded.nume,
                este_asigurat_cnas = excluded.este_asigurat_cnas,
                are_reteta_valida = excluded.are_reteta_valida,
                reducere_fidelitate = excluded.reducere_fidelitate,
                buget = excluded.buget;
        )SQL");
        st.bindText(1, idClient);
        st.bindText(2, nume);
        st.bindInt(3, esteAsiguratCnas ? 1 : 0);
        st.bindInt(4, areRetetaValida ? 1 : 0);
        st.bindDouble(5, reducereFidelitate);
        st.bindDouble(6, buget);
        st.exec();
    }

public:
    Client(BazaDateSQLite& db, const std::string& idClient, const std::string& nume, double buget,
           bool esteAsiguratCnas = true, bool areRetetaValida = false, double reducereFidelitate = 0.0)
        : db(db),
          idClient(idClient),
          nume(nume),
          esteAsiguratCnas(esteAsiguratCnas),
          areRetetaValida(areRetetaValida),
          reducereFidelitate(reducereFidelitate),
          buget(buget) {
        if (idClient.empty() || nume.empty()) {
            throw std::invalid_argument("ID-ul si numele clientului nu pot fi goale!");
        }
        if (buget < 0.0) {
            throw std::invalid_argument("Bugetul clientului nu poate fi negativ!");
        }
        if (reducereFidelitate < 0.0 || reducereFidelitate > 100.0) {
            throw std::invalid_argument("Reducerea de fidelitate trebuie sa fie intre 0% si 100%!");
        }
        salveazaInDB();
    }

    static Client incarcaDinDB(BazaDateSQLite& db, const std::string& idClient) {
        auto st = db.pregateste(
            "SELECT id_client, nume, este_asigurat_cnas, are_reteta_valida, reducere_fidelitate, buget "
            "FROM clienti WHERE id_client = ?;");
        st.bindText(1, idClient);
        if (st.step() == SQLITE_ROW) {
            return Client(db, st.colText(0), st.colText(1),
                          st.colInt(2) != 0, st.colInt(3) != 0,
                          st.colDouble(4), st.colDouble(5));
        }
        throw std::invalid_argument("Nu exista niciun client cu ID-ul '" + idClient + "' in baza de date!");
    }

    static std::vector<Client> getTotiClientii(BazaDateSQLite& db) {
        std::vector<Client> rez;
        auto st = db.pregateste(
            "SELECT id_client, nume, este_asigurat_cnas, are_reteta_valida, reducere_fidelitate, buget "
            "FROM clienti ORDER BY id_client ASC;");
        while (st.step() == SQLITE_ROW) {
            rez.push_back(Client(db, st.colText(0), st.colText(1),
                                 st.colInt(2) != 0, st.colInt(3) != 0,
                                 st.colDouble(4), st.colDouble(5)));
        }
        return rez;
    }

    void adaugaInCos(const Medicament& med, int cantitate = 1) {
        if (cantitate <= 0) {
            throw std::invalid_argument("Cantitatea adaugata in cos trebuie sa fie pozitiva!");
        }
        int medId = db.asiguraMedicament(med);
        auto st = db.pregateste(R"SQL(
            INSERT INTO cos_cumparaturi (client_id, medicament_id, cantitate)
            VALUES (?, ?, ?)
            ON CONFLICT(client_id, medicament_id) DO UPDATE SET
                cantitate = cantitate + excluded.cantitate;
        )SQL");
        st.bindText(1, idClient);
        st.bindInt(2, medId);
        st.bindInt(3, cantitate);
        st.exec();
    }

    void eliminaDinCos(const Medicament& med, int cantitate = 1) {
        if (cantitate <= 0) {
            throw std::invalid_argument("Cantitatea eliminata trebuie sa fie pozitiva!");
        }
        auto sel = db.pregateste(R"SQL(
            SELECT c.medicament_id, c.cantitate
            FROM cos_cumparaturi c
            JOIN medicamente m ON c.medicament_id = m.id
            WHERE c.client_id = ? AND m.nume = ? AND m.tip = ?;
        )SQL");
        sel.bindText(1, idClient);
        sel.bindText(2, med.getNume());
        sel.bindInt(3, static_cast<int>(med.getTipMedicament()));

        if (sel.step() == SQLITE_ROW) {
            int medId = sel.colInt(0);
            int cantActuala = sel.colInt(1);
            if (cantitate >= cantActuala) {
                auto del = db.pregateste("DELETE FROM cos_cumparaturi WHERE client_id = ? AND medicament_id = ?;");
                del.bindText(1, idClient);
                del.bindInt(2, medId);
                del.exec();
            } else {
                auto upd = db.pregateste(
                    "UPDATE cos_cumparaturi SET cantitate = cantitate - ? WHERE client_id = ? AND medicament_id = ?;");
                upd.bindInt(1, cantitate);
                upd.bindText(2, idClient);
                upd.bindInt(3, medId);
                upd.exec();
            }
        }
    }

    void golesteCos() {
        auto st = db.pregateste("DELETE FROM cos_cumparaturi WHERE client_id = ?;");
        st.bindText(1, idClient);
        st.exec();
    }

    void plateste(double suma) {
        if (suma < 0.0) {
            throw std::invalid_argument("Suma de plata nu poate fi negativa!");
        }
        if (suma > buget + 1e-9) {
            throw std::runtime_error("Fonduri insuficiente pentru clientul " + nume + "!");
        }
        buget = std::max(0.0, buget - suma);
        auto st = db.pregateste("UPDATE clienti SET buget = ? WHERE id_client = ?;");
        st.bindDouble(1, buget);
        st.bindText(2, idClient);
        st.exec();
    }

    void setAreRetetaValida(bool valoare) {
        areRetetaValida = valoare;
        salveazaInDB();
    }

    void adaugaBuget(double suma) {
        if (suma > 0.0) {
            buget += suma;
            salveazaInDB();
        }
    }

    CosCumparaturi getCos() const {
        std::vector<ItemCos> items;
        auto st = db.pregateste(R"SQL(
            SELECT m.pret, m.nume, m.tip, m.necesita_reteta, m.acoperire_cnas, c.cantitate
            FROM cos_cumparaturi c
            JOIN medicamente m ON c.medicament_id = m.id
            WHERE c.client_id = ?
            ORDER BY m.nume ASC;
        )SQL");
        st.bindText(1, idClient);
        while (st.step() == SQLITE_ROW) {
            Medicament med(st.colDouble(0), st.colText(1), st.colInt(2),
                           st.colInt(3) != 0, st.colDouble(4));
            items.push_back(ItemCos{med, st.colInt(5)});
        }
        return CosCumparaturi(std::move(items));
    }

    const std::string& getIdClient() const { return idClient; }
    const std::string& getNume() const { return nume; }
    bool getEsteAsiguratCnas() const { return esteAsiguratCnas; }
    bool getAreRetetaValida() const { return areRetetaValida; }
    double getReducereFidelitate() const { return reducereFidelitate; }
    double getBuget() const { return buget; }

    friend std::ostream& operator<<(std::ostream& os, const Client& c) {
        CosCumparaturi cosActual = c.getCos();
        os << "Client: " << c.nume << " (ID: " << c.idClient << ")"
           << " | CNAS: " << (c.esteAsiguratCnas ? "Da" : "Nu")
           << " | Reteta: " << (c.areRetetaValida ? "Da" : "Nu")
           << " | Fidelitate: " << c.reducereFidelitate << "%"
           << " | Buget: " << std::fixed << std::setprecision(2) << c.buget << " RON\n"
           << "Continut cos:\n" << cosActual;
        if (!cosActual.esteGol()) {
            os << "  -> Total estimat cu reduceri/CNAS: " << std::fixed << std::setprecision(2)
               << cosActual.calculeazaTotalFinal(c.esteAsiguratCnas, c.reducereFidelitate) << " RON";
        }
        return os;
    }
};

// Bon fiscal

struct LinieBon {
    std::string numeMedicament;
    TipMedicament tip;
    int cantitate;
    double pretUnitarCatalog;
    double pretUnitarFinal;
    double subtotal;

    friend std::ostream& operator<<(std::ostream& os, const LinieBon& l) {
        os << l.numeMedicament << " (" << tipMedicamentToString(l.tip) << "): "
           << l.cantitate << " x " << std::fixed << std::setprecision(2) << l.pretUnitarFinal
           << " RON (catalog: " << l.pretUnitarCatalog << " RON) = " << l.subtotal << " RON";
        return os;
    }
};

class BonFiscal {
private:
    int numarBon;
    std::string numeClient;
    std::chrono::year_month_day dataEmitere;
    LinieBon* linii;
    std::size_t numarLinii;
    double totalBrut;
    double totalEconomisit;
    double totalDePlata;

public:
    BonFiscal(int numarBon, const std::string& numeClient, std::chrono::year_month_day dataEmitere,
              const std::vector<LinieBon>& listaLinii, double totalBrut, double totalDePlata)
        : numarBon(numarBon),
          numeClient(numeClient),
          dataEmitere(dataEmitere),
          linii(nullptr),
          numarLinii(listaLinii.size()),
          totalBrut(totalBrut),
          totalEconomisit(std::max(0.0, totalBrut - totalDePlata)),
          totalDePlata(totalDePlata) {
        if (numarLinii > 0) {
            linii = new LinieBon[numarLinii];
            for (std::size_t i = 0; i < numarLinii; ++i) {
                linii[i] = listaLinii[i];
            }
        }
    }

    // copy
    BonFiscal(const BonFiscal& other)
        : numarBon(other.numarBon),
          numeClient(other.numeClient),
          dataEmitere(other.dataEmitere),
          linii(nullptr),
          numarLinii(other.numarLinii),
          totalBrut(other.totalBrut),
          totalEconomisit(other.totalEconomisit),
          totalDePlata(other.totalDePlata) {
        if (numarLinii > 0) {
            linii = new LinieBon[numarLinii];
            for (std::size_t i = 0; i < numarLinii; ++i) {
                linii[i] = other.linii[i];
            }
        }
    }

    // copy assignement
    BonFiscal& operator=(const BonFiscal& other) {
        if (this == &other) {
            return *this;
        }

        LinieBon* liniiNoi = nullptr;
        if (other.numarLinii > 0) {
            liniiNoi = new LinieBon[other.numarLinii];
            for (std::size_t i = 0; i < other.numarLinii; ++i) {
                liniiNoi[i] = other.linii[i];
            }
        }

        delete[] linii;
        linii = liniiNoi;
        numarLinii = other.numarLinii;
        numarBon = other.numarBon;
        numeClient = other.numeClient;
        dataEmitere = other.dataEmitere;
        totalBrut = other.totalBrut;
        totalEconomisit = other.totalEconomisit;
        totalDePlata = other.totalDePlata;

        return *this;
    }

    // destructor
    ~BonFiscal() {
        delete[] linii;
        linii = nullptr;
    }

    int getNumarBon() const { return numarBon; }
    /*const std::string& getNumeClient() const { return numeClient; } momentan neutilizata*/
    std::size_t getNumarLinii() const { return numarLinii; }
    /*double getTotalDePlata() const { return totalDePlata; } momentan neutilizata*/

    friend std::ostream& operator<<(std::ostream& os, const BonFiscal& b) {
        os << "================ BON FISCAL #" << b.numarBon << " ================\n"
           << "Data: " << formatData(b.dataEmitere) << " | Client: " << b.numeClient << "\n"
           << "------------------------------------------------\n";
        for (std::size_t i = 0; i < b.numarLinii; ++i) {
            os << "  " << b.linii[i] << "\n";
        }
        os << "------------------------------------------------\n"
           << "Total brut:        " << std::fixed << std::setprecision(2) << b.totalBrut << " RON\n"
           << "Economie CNAS/Card:" << b.totalEconomisit << " RON\n"
           << "TOTAL ACHITAT:     " << b.totalDePlata << " RON\n"
           << "================================================\n";
        return os;
    }
};

class CasaDeMarcat {
private:
    BazaDateSQLite& db;
    std::string numeFarmacie;

public:
    CasaDeMarcat(BazaDateSQLite& db, const std::string& numeFarmacie)
        : db(db), numeFarmacie(numeFarmacie) {
        if (numeFarmacie.empty()) {
            throw std::invalid_argument("Numele farmaciei nu poate fi gol!");
        }
    }

    BonFiscal proceseazaVanzare(Client& client, Inventar& inventar) {
        TranzactieSQLite tranzactie(db);

        CosCumparaturi cos = client.getCos();
        if (cos.esteGol()) {
            throw std::logic_error("Vanzare respinsa: cosul clientului " + client.getNume() + " este gol!");
        }

        for (const auto& item : cos.getProduse()) {
            if (item.medicament.getNecesitaRetetaMedicala() && !client.getAreRetetaValida()) {
                throw std::runtime_error(
                    "Vanzare respinsa: medicamentul '" + item.medicament.getNume() +
                    "' necesita reteta medicala, iar clientul " + client.getNume() + " nu are reteta!");
            }
        }

        for (const auto& item : cos.getProduse()) {
            if (!inventar.areStocDisponibil(item.medicament, item.cantitate)) {
                int disponibil = inventar.getStocDisponibil(item.medicament);
                throw std::runtime_error(
                    "Vanzare respinsa: stoc insuficient pentru '" + item.medicament.getNume() +
                    "' (cerut: " + std::to_string(item.cantitate) +
                    ", disponibil: " + std::to_string(disponibil) + ")!");
            }
        }

        std::vector<LinieBon> linii;
        double totalBrut = 0.0;
        double totalDePlata = 0.0;

        for (const auto& item : cos.getProduse()) {
            double pretCatalog = item.medicament.getPret();
            double pretFinal = item.medicament.calculeazaPretFinal(
                client.getEsteAsiguratCnas(), client.getReducereFidelitate());
            double subtotal = std::round(pretFinal * item.cantitate * 100.0) / 100.0;

            totalBrut += pretCatalog * item.cantitate;
            totalDePlata += subtotal;

            linii.push_back(LinieBon{
                item.medicament.getNume(),
                item.medicament.getTipMedicament(),
                item.cantitate,
                pretCatalog,
                pretFinal,
                subtotal
            });
        }
        totalDePlata = std::round(totalDePlata * 100.0) / 100.0;

        if (client.getBuget() < totalDePlata) {
            std::ostringstream oss;
            oss << "Vanzare respinsa: buget insuficient pentru " << client.getNume()
                << " (necesar: " << std::fixed << std::setprecision(2) << totalDePlata
                << " RON, disponibil: " << client.getBuget() << " RON)!";
            throw std::runtime_error(oss.str());
        }

        for (const auto& item : cos.getProduse()) {
            inventar.consumaStoc(item.medicament, item.cantitate);
        }
        inventar.eliminaLoturiEpuizate();

        client.plateste(totalDePlata);
        client.golesteCos();

        auto azi = dataCurentaSistema();
        double totalEconomisit = std::max(0.0, totalBrut - totalDePlata);

        {
            auto insBon = db.pregateste(R"SQL(
                INSERT INTO bonuri_fiscale
                    (client_id, nume_client, data_emitere, total_brut, total_economisit, total_de_plata)
                VALUES (?, ?, ?, ?, ?, ?);
            )SQL");
            insBon.bindText(1, client.getIdClient());
            insBon.bindText(2, client.getNume());
            insBon.bindText(3, formatData(azi));
            insBon.bindDouble(4, totalBrut);
            insBon.bindDouble(5, totalEconomisit);
            insBon.bindDouble(6, totalDePlata);
            insBon.exec();
        }

        int numarBonGenerat = static_cast<int>(db.ultimulIdInserat());

        for (const auto& l : linii) {
            auto insLinie = db.pregateste(R"SQL(
                INSERT INTO linii_bon
                    (numar_bon, nume_medicament, tip, cantitate, pret_catalog, pret_final, subtotal)
                VALUES (?, ?, ?, ?, ?, ?, ?);
            )SQL");
            insLinie.bindInt(1, numarBonGenerat);
            insLinie.bindText(2, l.numeMedicament);
            insLinie.bindInt(3, static_cast<int>(l.tip));
            insLinie.bindInt(4, l.cantitate);
            insLinie.bindDouble(5, l.pretUnitarCatalog);
            insLinie.bindDouble(6, l.pretUnitarFinal);
            insLinie.bindDouble(7, l.subtotal);
            insLinie.exec();
        }

        tranzactie.commit();

        return BonFiscal(numarBonGenerat, client.getNume(), azi, linii, totalBrut, totalDePlata);
    }

    friend std::ostream& operator<<(std::ostream& os, const CasaDeMarcat& casa) {
        int numarBonuri = 0;
        double totalIncasari = 0.0;

        {
            auto st = casa.db.pregateste(
                "SELECT COUNT(*), COALESCE(SUM(total_de_plata), 0.0) FROM bonuri_fiscale;");
            if (st.step() == SQLITE_ROW) {
                numarBonuri = st.colInt(0);
                totalIncasari = st.colDouble(1);
            }
        }

        os << "=== CASA DE MARCAT SQLITE (" << casa.numeFarmacie << ") ===\n"
           << "Bonuri emise: " << numarBonuri << "\n"
           << "Total incasari: " << std::fixed << std::setprecision(2)
           << totalIncasari << " RON";

        if (numarBonuri > 0) {
            os << "\nRezumat bonuri din baza de date:";
            auto st = casa.db.pregateste(
                "SELECT numar_bon, nume_client, data_emitere, total_de_plata "
                "FROM bonuri_fiscale ORDER BY numar_bon ASC;");
            while (st.step() == SQLITE_ROW) {
                os << "\n  * Bon #" << st.colInt(0)
                   << " | Data: " << st.colText(2)
                   << " | Client: " << st.colText(1)
                   << " | Achitat: " << std::fixed << std::setprecision(2)
                   << st.colDouble(3) << " RON";
            }
        }
        return os;
    }
};


Medicament citesteMedicamentDinCin() {
    std::string nume = citesteLinie("  Nume medicament: ");
    TipMedicament tip = citesteTipMedicament("  Tip medicament (0-16 sau nume, ex: COMPRIMATE): ");
    double pret = citesteDouble("  Pret catalog (RON): ", 0.01, 1000000.0);
    bool necesitaReteta = citesteBool("  Necesita reteta medicala? (1 = Da, 0 = Nu): ");
    double acoperireCnas = citesteDouble("  Procent acoperire CNAS (0 - 100): ", 0.0, 100.0);
    return Medicament(pret, nume, tip, necesitaReteta, acoperireCnas);
}

void afiseazaMeniuPrincipal() {
    std::cout << "\n======================================================\n"
              << "             GESTIUNE FARMACIE (SQLite3)              \n"
              << "======================================================\n"
              << "  1. Adauga medicament in catalog\n"
              << "  2. Adauga lot nou in inventar\n"
              << "  3. Marcheaza un lot ca RETRAS\n"
              << "  4. Sterge lot / Elimina loturi epuizate\n"
              << "  5. Afiseaza inventarul complet\n"
              << "  6. Verifica loturi care expira in urmatoarele N zile\n"
              << "  7. Inregistreaza / actualizeaza client\n"
              << "  8. Afiseaza toti clientii si cosurile lor\n"
              << "  9. Gestioneaza cosul unui client (adauga/elimina/goleste)\n"
              << " 10. Actualizeaza reteta / bugetul unui client\n"
              << " 11. Proceseaza vanzare la casa de marcat (Checkout)\n"
              << " 12. Afiseaza raportul casei de marcat\n"
              << " 13. Afiseaza catalogul de medicamente\n"
              << "  0. Iesire\n"
              << "------------------------------------------------------\n";
}


int main() {
    try {
        std::string fisierDb = citesteLinie("Fisier baza de date SQLite (ex: farmacie.db sau :memory:): ");
        std::string numeFarmacie = citesteLinie("Nume farmacie: ");

        BazaDateSQLite db(fisierDb);
        Inventar inventar(db);
        CasaDeMarcat casa(db, numeFarmacie);

        std::cout << "Conectat cu succes: " << db << "\n";

        bool ruleaza = true;
        while (ruleaza) {
            afiseazaMeniuPrincipal();
            int optiune = citesteInt("Alege optiunea: ", 0, 13);

            try {
                switch (optiune) {
                    case 0: {
                        ruleaza = false;
                        std::cout << "Oprire program. Date salvate in: " << db.getCaleFisier() << "\n";
                        break;
                    }

                    case 1: {
                        std::cout << "\n--- Adaugare medicament in catalog ---\n";
                        Medicament med = citesteMedicamentDinCin();
                        int id = db.asiguraMedicament(med);
                        std::cout << "  [OK] Salvat in catalog cu ID #" << id << ": " << med << "\n";
                        break;
                    }

                    case 2: {
                        std::cout << "\n--- Adaugare lot nou in inventar ---\n";
                        std::string numarLot = citesteLinie("  Numar lot: ");
                        int modSelectie = citesteInt(
                            "  Selectie medicament (1 = existent din catalog dupa ID, 2 = introduc medicament nou): ",
                            1, 2);

                        std::optional<Medicament> medAles;
                        if (modSelectie == 1) {
                            int idMed = citesteInt("  ID medicament din catalog: ", 1, 1000000);
                            medAles = db.cautaMedicamentDupaId(idMed);
                        } else {
                            medAles = citesteMedicamentDinCin();
                        }

                        int cantitate = citesteInt("  Cantitate cutii in lot: ", 1, 1000000);
                        auto dataExp = citesteData("  Data expirare (YYYY-MM-DD): ");
                        bool retras = citesteBool("  Lotul este marcat ca retras? (1 = Da, 0 = Nu): ");

                        Lot lot(numarLot, cantitate, dataExp, *medAles, retras);
                        inventar.adaugaLot(lot);
                        std::cout << "  [OK] Lot salvat in SQLite: " << lot << "\n";
                        break;
                    }

                    case 3: {
                        std::cout << "\n--- Retragere lot ---\n";
                        std::string nr = citesteLinie("  Numar lot de retras: ");
                        inventar.retrageLot(nr);
                        std::cout << "  [OK] Lotul #" << nr << " a fost marcat ca RETRAS.\n";
                        break;
                    }

                    case 4: {
                        std::cout << "\n--- Stergere loturi ---\n";
                        int mod = citesteInt("  Mod (1 = sterge un lot dupa numar, 2 = elimina toate loturile epuizate): ", 1, 2);
                        if (mod == 1) {
                            std::string nr = citesteLinie("  Numar lot de sters: ");
                            inventar.stergeLot(nr);
                            std::cout << "  [OK] Lotul #" << nr << " a fost sters.\n";
                        } else {
                            int sterse = inventar.eliminaLoturiEpuizate();
                            std::cout << "  [OK] Loturi epuizate eliminate: " << sterse << "\n";
                        }
                        break;
                    }

                    case 5: {
                        std::cout << "\n" << inventar << "\n";
                        break;
                    }

                    case 6: {
                        int zile = citesteInt("  Numar de zile pentru alerta expirare: ", 0, 3650);
                        auto aproapeExp = inventar.getStocuriAproapeExpirate(zile);
                        if (aproapeExp.empty()) {
                            std::cout << "  Niciun lot activ nu expira in urmatoarele " << zile << " zile.\n";
                        } else {
                            std::cout << "  [!] Loturi care expira in urmatoarele " << zile << " zile:\n";
                            for (const auto& l : aproapeExp) {
                                std::cout << "    ! " << l << "\n";
                            }
                        }
                        break;
                    }

                    case 7: {
                        std::cout << "\n--- Inregistrare / Actualizare client ---\n";
                        std::string id = citesteLinie("  ID client: ");
                        std::string nume = citesteLinie("  Nume complet client: ");
                        double buget = citesteDouble("  Buget disponibil (RON): ", 0.0, 1000000.0);
                        bool cnas = citesteBool("  Este asigurat CNAS? (1 = Da, 0 = Nu): ");
                        bool reteta = citesteBool("  Detine reteta medicala valida? (1 = Da, 0 = Nu): ");
                        double fidelitate = citesteDouble("  Reducere card fidelitate (0 - 100%): ", 0.0, 100.0);

                        Client client(db, id, nume, buget, cnas, reteta, fidelitate);
                        std::cout << "  [OK] Client salvat in SQLite:\n" << client << "\n";
                        break;
                    }

                    case 8: {
                        std::cout << "\n--- Lista clienti din SQLite ---\n";
                        auto clienti = Client::getTotiClientii(db);
                        if (clienti.empty()) {
                            std::cout << "  [Nu exista clienti inregistrati]\n";
                        } else {
                            for (const auto& c : clienti) {
                                std::cout << c << "\n------------------------------------------------------\n";
                            }
                        }
                        break;
                    }

                    case 9: {
                        std::cout << "\n--- Gestionare cos cumparaturi ---\n";
                        std::string idClient = citesteLinie("  ID client: ");
                        Client client = Client::incarcaDinDB(db, idClient);

                        int act = citesteInt("  Actiune (1 = adauga in cos, 2 = elimina din cos, 3 = goleste cos): ", 1, 3);
                        if (act == 1 || act == 2) {
                            int idMed = citesteInt("  ID medicament din catalog: ", 1, 1000000);
                            Medicament med = db.cautaMedicamentDupaId(idMed);
                            int cant = citesteInt("  Cantitate: ", 1, 10000);

                            if (act == 1) {
                                client.adaugaInCos(med, cant);
                                std::cout << "  [OK] Produs adaugat in cos.\n";
                            } else {
                                client.eliminaDinCos(med, cant);
                                std::cout << "  [OK] Produs eliminat din cos.\n";
                            }
                        } else {
                            client.golesteCos();
                            std::cout << "  [OK] Cosul clientului a fost golit.\n";
                        }

                        std::cout << client << "\n";
                        break;
                    }

                    case 10: {
                        std::cout << "\n--- Actualizare date client ---\n";
                        std::string idClient = citesteLinie("  ID client: ");
                        Client client = Client::incarcaDinDB(db, idClient);

                        bool reteta = citesteBool("  Are reteta medicala valida? (1 = Da, 0 = Nu): ");
                        client.setAreRetetaValida(reteta);

                        double supliment = citesteDouble("  Suma de adaugat la buget (0 daca nu): ", 0.0, 1000000.0);
                        if (supliment > 0.0) {
                            client.adaugaBuget(supliment);
                        }

                        std::cout << "  [OK] Client actualizat:\n" << client << "\n";
                        break;
                    }

                    case 11: {
                        std::cout << "\n--- Procesare vanzare (Checkout) ---\n";
                        std::string idClient = citesteLinie("  ID client pentru incasare: ");
                        Client client = Client::incarcaDinDB(db, idClient);

                        BonFiscal bon = casa.proceseazaVanzare(client, inventar);
                        std::cout << "\n" << bon;

                        // Demonstratie Rule of Three pe bonul emis
                        BonFiscal copieArhiva(bon);
                        BonFiscal backup = bon;
                        backup = copieArhiva;
                        std::cout << "  [Arhiva] Bon #" << backup.getNumarBon()
                                  << " copiat (" << backup.getNumarLinii()
                                  << " linii) | Buget ramas client: "
                                  << std::fixed << std::setprecision(2) << client.getBuget() << " RON\n";
                        break;
                    }

                    case 12: {
                        std::cout << "\n" << casa << "\n";
                        break;
                    }
                    case 13: {
                        for (const auto& mc : db.getToateMedicamentele()){
                            std::cout << "\n" << mc << "\n";
                        }
                        break;
                    }
                }
            } catch (const IesireDinProgram&) {
                throw;
            } catch (const std::exception& e) {
                std::cout << "\n  [EROARE OPERATIUNE] " << e.what() << "\n";
            }
        }
    } catch (const IesireDinProgram&) {
        std::cout << "\nFlux std::cin incheiat. Oprire program.\n";
    } catch (const std::exception& ex) {
        std::cerr << "Eroare fatala: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}