# Scraper de Subwoofers - loudspeakerdatabase.com

## 📋 Description

Ce projet contient plusieurs scripts pour extraire les données des caissons de basses (subwoofers) encore en vente sur loudspeakerdatabase.com.

## 🔧 Scripts Disponibles

### 1. `scrape_subwoofers.py` - Version Requests/BeautifulSoup
Script léger utilisant requests et BeautifulSoup.
- ✅ Rapide et léger
- ❌ Bloqué par la protection anti-scraping du site (erreur 403)

### 2. `scrape_subwoofers_selenium.py` - Version Selenium
Script utilisant Selenium avec un navigateur Chrome headless.
- ✅ Simule un vrai navigateur
- ✅ Peut contourner certaines protections
- ❌ Nécessite Chrome/Chromium installé

### 3. `scrape_subwoofers_manual.py` - Version Manuel
Script interactif qui guide l'utilisateur pour extraire manuellement les données.
- ✅ Fonctionne toujours
- ✅ Pas de dépendances complexes
- ⚠️ Nécessite une intervention manuelle

## 📦 Installation

### Prérequis

```bash
# Installer Python 3.7+
python3 --version

# Installer les dépendances
pip install -r requirements.txt
```

### Pour utiliser Selenium (recommandé si possible)

```bash
# Sur Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y chromium-browser chromium-chromedriver

# Sur macOS
brew install chromium

# Vérifier l'installation
chromium-browser --version
```

## 🚀 Utilisation

### Méthode 1: Avec Selenium (si Chrome est installé)

```bash
python3 scrape_subwoofers_selenium.py
```

Les résultats seront sauvegardés dans:
- `subwoofers_available.json` - Subwoofers en vente
- `subwoofers_available.csv` - Format CSV
- `subwoofers_all.json` - Tous les subwoofers trouvés
- `homepage_exploration.json` - Analyse de la structure du site

### Méthode 2: Manuel (toujours fonctionnel)

```bash
python3 scrape_subwoofers_manual.py
```

Suivez les instructions à l'écran pour:
1. Naviguer vers le site web
2. Copier/coller le HTML ou les données
3. Le script extraira les informations

### Méthode 3: Requests (bloqué actuellement)

```bash
python3 scrape_subwoofers.py
```

⚠️ Cette méthode est actuellement bloquée par le site (erreur 403).

## 📊 Format des Données

Les données extraites contiennent (selon disponibilité):
- **name**: Nom du subwoofer
- **manufacturer**: Fabricant
- **model**: Modèle
- **url**: Lien vers la fiche produit
- **specifications**: Caractéristiques techniques
- **availability**: Statut de disponibilité
- **price**: Prix (si disponible)

Exemple:
```json
{
  "name": "JBL 2245H",
  "manufacturer": "JBL",
  "model": "2245H",
  "url": "https://loudspeakerdatabase.com/...",
  "specifications": {...},
  "availability": "Available"
}
```

## 🛠️ Dépannage

### Erreur 403 (Forbidden)
Le site bloque les requêtes automatiques. Solutions:
1. Utiliser Selenium avec un navigateur réel
2. Ajouter des délais entre les requêtes
3. Utiliser un proxy ou VPN
4. Extraire manuellement avec `scrape_subwoofers_manual.py`

### Selenium ne trouve pas Chrome
```bash
# Installer Chromium
sudo apt-get install chromium-browser

# Ou utiliser le script manuel
python3 scrape_subwoofers_manual.py
```

### Pas de données extraites
Le site a peut-être changé de structure. Vérifier:
1. `homepage_exploration.json` pour comprendre la structure
2. Adapter les sélecteurs CSS/XPath dans le script
3. Utiliser le mode manuel pour extraire les données

## 📝 Notes

- **Respectez les conditions d'utilisation** du site web
- **Ajoutez des délais** entre les requêtes pour ne pas surcharger le serveur
- **Ne redistribuez pas** les données sans autorisation
- Les scripts filtrent automatiquement les subwoofers marqués comme "discontinued"

## 🔄 Amélioration Continue

Le site web peut changer sa structure. Si les scripts ne fonctionnent plus:

1. Exécutez d'abord le script Selenium pour obtenir `homepage_exploration.json`
2. Analysez la structure du site
3. Adaptez les sélecteurs dans les scripts
4. En dernier recours, utilisez le script manuel

## 📧 Support

Pour toute question ou problème:
1. Vérifiez que les dépendances sont installées
2. Consultez les messages d'erreur
3. Essayez la méthode manuelle
4. Ouvrez une issue avec les logs d'erreur

## 📄 Licence

Scripts fournis à des fins éducatives uniquement.
