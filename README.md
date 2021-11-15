# Inverted Search Engine - Project_1

![Build Status](https://github.com/chatziko-k08/lecture-code/workflows/run-tests/badge.svg)

Έχουν υλοποιηθεί όλες οι ζητούμενες συναρτήσεις. 
Για το deduplication των query words υπάρχει η συνάρτηση read_queries η οποία διαβάζει κάθε word, κάνει το αντίστοιχο deduplicate και επιστρέφει το entry list πάνω στο οποίο θα χτιστεί το BK Tree . Αντίστοιχα, για τις λέξεις των documents υπάρχει η read_documents η οποία κάνει την αντίστοιχη δουλεία, δηλαδή διάβασμα και deduplicate για κάθε αρχείο επιστρέφοντας τον πίνακα entry lists που θα ελεγχθούν για το κάθε document.
Για την δημιουργία του ΒΚ Tree διαβάζονται μια μια οι λέξεις από το entry list και δημιουργείτε αναδρομικά μια δομή ΒΚ Tree
Τέλος για κάθε λέξη απο τα documents γίνεται αναδρομική αναζήτηση πάνω στο bk tree για να βρεθούν οι λέξεις που έχουν απόσταση μικρότερη απο την ζητούμενη.


### Usage Example

./project1 match_type threshold

- match_type 0 = HUMMING_DIST
- match_type 1 = EDIT_DIST

```bash
    make
    ./project1 1 2
```

### Contributors

[Stathis Trigas](https://github.com/stathis99)

[Dimitris Tsiamouras](https://github.com/sdi1700168)


