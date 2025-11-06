#!/usr/bin/env python3
"""
Script pour scraper les données des caissons de basses (subwoofers)
encore en vente sur loudspeakerdatabase.com avec Selenium
"""

import time
import json
import re
from typing import List, Dict
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service
from selenium.common.exceptions import TimeoutException, NoSuchElementException
from bs4 import BeautifulSoup

class SubwooferScraperSelenium:
    def __init__(self, headless=True):
        self.base_url = "https://loudspeakerdatabase.com"
        self.driver = self._setup_driver(headless)

    def _setup_driver(self, headless=True):
        """Configure le driver Selenium Chrome"""
        chrome_options = Options()

        if headless:
            chrome_options.add_argument("--headless=new")

        chrome_options.add_argument("--no-sandbox")
        chrome_options.add_argument("--disable-dev-shm-usage")
        chrome_options.add_argument("--disable-blink-features=AutomationControlled")
        chrome_options.add_argument("--window-size=1920,1080")
        chrome_options.add_experimental_option("excludeSwitches", ["enable-automation"])
        chrome_options.add_experimental_option('useAutomationExtension', False)

        # User agent réaliste
        chrome_options.add_argument('user-agent=Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36')

        driver = webdriver.Chrome(options=chrome_options)

        # Masquer l'automatisation
        driver.execute_cdp_cmd('Network.setUserAgentOverride', {
            "userAgent": 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36'
        })
        driver.execute_script("Object.defineProperty(navigator, 'webdriver', {get: () => undefined})")

        return driver

    def get_page(self, url: str, wait_time=10) -> BeautifulSoup:
        """Charge une page et retourne le HTML parsé"""
        try:
            print(f"Chargement de {url}...")
            self.driver.get(url)

            # Attendre que la page soit chargée
            time.sleep(wait_time)

            # Récupérer le HTML
            html = self.driver.page_source
            return BeautifulSoup(html, 'html.parser')
        except Exception as e:
            print(f"Erreur lors du chargement de {url}: {e}")
            return None

    def explore_homepage(self) -> Dict:
        """Explore la page d'accueil pour comprendre la structure"""
        print("\n=== Exploration de la page d'accueil ===")
        soup = self.get_page(self.base_url, wait_time=5)

        if not soup:
            return {"error": "Impossible de charger la page d'accueil"}

        info = {
            "title": soup.find('title').get_text() if soup.find('title') else "N/A",
            "links": [],
            "navigation": []
        }

        # Chercher les liens de navigation
        nav_elements = soup.find_all(['nav', 'header', 'menu'])
        for nav in nav_elements:
            links = nav.find_all('a', href=True)
            for link in links:
                text = link.get_text().strip()
                href = link['href']
                if text:
                    info["navigation"].append({"text": text, "href": href})

        # Chercher tous les liens qui pourraient mener aux subwoofers
        all_links = soup.find_all('a', href=True)
        keywords = ['subwoofer', 'woofer', 'bass', 'speaker', 'database', 'browse', 'search']

        for link in all_links:
            text = link.get_text().strip().lower()
            href = link['href'].lower()

            for keyword in keywords:
                if keyword in text or keyword in href:
                    full_url = link['href'] if link['href'].startswith('http') else self.base_url + link['href']
                    info["links"].append({
                        "text": link.get_text().strip(),
                        "href": full_url,
                        "keyword_matched": keyword
                    })
                    break

        return info

    def scrape_subwoofer_table(self, soup: BeautifulSoup) -> List[Dict]:
        """Extrait les données d'une table de subwoofers"""
        subwoofers = []

        # Chercher toutes les tables
        tables = soup.find_all('table')

        for table in tables:
            # Trouver les en-têtes
            headers = []
            header_row = table.find('thead') or table.find('tr')

            if header_row:
                for th in header_row.find_all(['th', 'td']):
                    headers.append(th.get_text().strip())

            # Parser les lignes de données
            tbody = table.find('tbody') or table
            rows = tbody.find_all('tr')[1:] if headers else tbody.find_all('tr')

            for row in rows:
                cells = row.find_all(['td', 'th'])
                if len(cells) > 0:
                    data = {}

                    for i, cell in enumerate(cells):
                        header = headers[i] if i < len(headers) else f'column_{i}'
                        data[header] = cell.get_text().strip()

                        # Chercher les liens
                        link = cell.find('a', href=True)
                        if link:
                            full_url = link['href'] if link['href'].startswith('http') else self.base_url + link['href']
                            data[f'{header}_url'] = full_url

                    if data:
                        subwoofers.append(data)

        return subwoofers

    def scrape_subwoofer_cards(self, soup: BeautifulSoup) -> List[Dict]:
        """Extrait les données de cartes/divs de produits"""
        subwoofers = []

        # Patterns de classes communes
        class_patterns = [
            re.compile(r'.*product.*', re.IGNORECASE),
            re.compile(r'.*item.*', re.IGNORECASE),
            re.compile(r'.*speaker.*', re.IGNORECASE),
            re.compile(r'.*card.*', re.IGNORECASE)
        ]

        for pattern in class_patterns:
            elements = soup.find_all(['div', 'article', 'li'], class_=pattern)

            for elem in elements:
                data = {}

                # Chercher le titre/nom
                title = elem.find(['h1', 'h2', 'h3', 'h4', 'h5', 'a'])
                if title:
                    data['name'] = title.get_text().strip()

                    link = title if title.name == 'a' else title.find('a', href=True)
                    if link and link.get('href'):
                        full_url = link['href'] if link['href'].startswith('http') else self.base_url + link['href']
                        data['url'] = full_url

                # Extraire d'autres informations
                info_fields = ['price', 'manufacturer', 'model', 'brand', 'status', 'availability']
                for field in info_fields:
                    field_elem = elem.find(class_=re.compile(f'.*{field}.*', re.IGNORECASE))
                    if field_elem:
                        data[field] = field_elem.get_text().strip()

                # Extraire le texte complet pour analyse
                data['full_text'] = elem.get_text(separator=' ', strip=True)

                if data.get('name'):
                    subwoofers.append(data)

        return subwoofers

    def scrape_page_for_subwoofers(self, url: str) -> List[Dict]:
        """Scrape une page complète à la recherche de subwoofers"""
        soup = self.get_page(url, wait_time=8)
        if not soup:
            return []

        # Essayer différentes méthodes d'extraction
        subwoofers = []

        # Méthode 1: Tables
        table_data = self.scrape_subwoofer_table(soup)
        if table_data:
            subwoofers.extend(table_data)
            print(f"  ✓ Trouvé {len(table_data)} entrées dans des tables")

        # Méthode 2: Cartes/Divs
        card_data = self.scrape_subwoofer_cards(soup)
        if card_data:
            subwoofers.extend(card_data)
            print(f"  ✓ Trouvé {len(card_data)} entrées dans des cartes")

        # Dédupliquer
        seen = set()
        unique_subwoofers = []
        for sub in subwoofers:
            identifier = sub.get('name', '') + sub.get('url', '')
            if identifier and identifier not in seen:
                seen.add(identifier)
                unique_subwoofers.append(sub)

        return unique_subwoofers

    def filter_available_subwoofers(self, subwoofers: List[Dict]) -> List[Dict]:
        """Filtre pour ne garder que les subwoofers en vente"""
        available = []
        discontinued_keywords = [
            'discontinued', 'arrêté', 'obsolete', 'not available',
            'out of stock', 'épuisé', 'unavailable', 'retired'
        ]

        for sub in subwoofers:
            is_discontinued = False

            # Vérifier tous les champs
            for key, value in sub.items():
                if isinstance(value, str):
                    value_lower = value.lower()
                    if any(keyword in value_lower for keyword in discontinued_keywords):
                        is_discontinued = True
                        break

            if not is_discontinued:
                available.append(sub)

        return available

    def scrape_all_subwoofers(self) -> List[Dict]:
        """Point d'entrée principal pour scraper tous les subwoofers"""
        all_subwoofers = []

        # Explorer la page d'accueil
        homepage_info = self.explore_homepage()

        # Sauvegarder l'exploration
        with open('homepage_exploration.json', 'w', encoding='utf-8') as f:
            json.dump(homepage_info, f, indent=2, ensure_ascii=False)
        print(f"\nInformations sur la page d'accueil sauvegardées dans homepage_exploration.json")

        # URLs à essayer
        urls_to_try = set([self.base_url])

        # Ajouter les liens trouvés
        if 'links' in homepage_info:
            for link_info in homepage_info['links'][:20]:  # Limiter à 20 pour ne pas surcharger
                urls_to_try.add(link_info['href'])

        # URLs communes
        common_paths = [
            '/Subwoofers', '/subwoofers', '/woofers',
            '/database', '/browse', '/search',
            '/speakers/subwoofers', '/drivers/subwoofers'
        ]

        for path in common_paths:
            urls_to_try.add(self.base_url + path)

        # Scraper chaque URL
        print(f"\n=== Scraping de {len(urls_to_try)} URLs ===")
        for i, url in enumerate(urls_to_try, 1):
            print(f"\n[{i}/{len(urls_to_try)}] {url}")
            subwoofers = self.scrape_page_for_subwoofers(url)

            if subwoofers:
                all_subwoofers.extend(subwoofers)
                print(f"  → Total partiel: {len(all_subwoofers)} subwoofers")

            # Pause pour ne pas surcharger
            time.sleep(3)

        return all_subwoofers

    def save_to_json(self, data: List[Dict], filename: str):
        """Sauvegarde en JSON"""
        with open(filename, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=2, ensure_ascii=False)
        print(f"\n✓ Données sauvegardées dans {filename}")

    def save_to_csv(self, data: List[Dict], filename: str):
        """Sauvegarde en CSV"""
        if not data:
            print("Aucune donnée à sauvegarder")
            return

        import csv

        all_keys = set()
        for item in data:
            all_keys.update(item.keys())

        with open(filename, 'w', newline='', encoding='utf-8') as f:
            writer = csv.DictWriter(f, fieldnames=sorted(all_keys))
            writer.writeheader()
            writer.writerows(data)

        print(f"✓ Données sauvegardées dans {filename}")

    def close(self):
        """Ferme le navigateur"""
        if self.driver:
            self.driver.quit()

def main():
    print("=" * 70)
    print("  Scraper de caissons de basses - loudspeakerdatabase.com (Selenium)")
    print("=" * 70)

    scraper = None
    try:
        scraper = SubwooferScraperSelenium(headless=True)

        # Scraper tous les subwoofers
        print("\n🔍 Démarrage du scraping avec Selenium...")
        all_subwoofers = scraper.scrape_all_subwoofers()

        print(f"\n📊 Résultats:")
        print(f"  • Total trouvé: {len(all_subwoofers)} subwoofers")

        # Filtrer pour ne garder que ceux en vente
        available_subwoofers = scraper.filter_available_subwoofers(all_subwoofers)
        print(f"  • Encore en vente: {len(available_subwoofers)} subwoofers")

        # Sauvegarder
        if available_subwoofers:
            scraper.save_to_json(available_subwoofers, 'subwoofers_available.json')
            scraper.save_to_csv(available_subwoofers, 'subwoofers_available.csv')

            # Afficher échantillon
            print("\n📋 Échantillon des données (3 premiers):")
            for i, sub in enumerate(available_subwoofers[:3], 1):
                print(f"\n  [{i}] {sub.get('name', 'N/A')}")
                for key, value in list(sub.items())[:5]:
                    if len(str(value)) < 100:
                        print(f"      • {key}: {value}")

        # Sauvegarder tous les résultats
        scraper.save_to_json(all_subwoofers, 'subwoofers_all.json')

        print("\n" + "=" * 70)
        print("✅ Scraping terminé avec succès!")
        print("=" * 70)

    except Exception as e:
        print(f"\n❌ Erreur: {e}")
        import traceback
        traceback.print_exc()

    finally:
        if scraper:
            scraper.close()

if __name__ == "__main__":
    main()
