#!/usr/bin/env python3
"""
Script pour scraper les données des caissons de basses (subwoofers)
encore en vente sur loudspeakerdatabase.com
"""

import requests
from bs4 import BeautifulSoup
import json
import time
from typing import List, Dict
import re

class SubwooferScraper:
    def __init__(self):
        self.base_url = "https://loudspeakerdatabase.com"
        self.headers = {
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36',
            'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',
            'Accept-Language': 'fr-FR,fr;q=0.9,en-US;q=0.8,en;q=0.7',
            'Accept-Encoding': 'gzip, deflate, br',
            'Connection': 'keep-alive',
            'Upgrade-Insecure-Requests': '1'
        }
        self.session = requests.Session()
        self.session.headers.update(self.headers)

    def get_page(self, url: str) -> BeautifulSoup:
        """Récupère et parse une page"""
        try:
            response = self.session.get(url, timeout=30)
            response.raise_for_status()
            return BeautifulSoup(response.content, 'html.parser')
        except requests.exceptions.RequestException as e:
            print(f"Erreur lors de la récupération de {url}: {e}")
            return None

    def find_subwoofer_pages(self) -> List[str]:
        """Trouve les pages de subwoofers sur le site"""
        soup = self.get_page(self.base_url)
        if not soup:
            return []

        # Chercher les liens vers les subwoofers/woofers
        subwoofer_links = []

        # Différentes variations possibles
        patterns = [
            re.compile(r'.*subwoofer.*', re.IGNORECASE),
            re.compile(r'.*woofer.*', re.IGNORECASE),
            re.compile(r'.*bass.*', re.IGNORECASE)
        ]

        for link in soup.find_all('a', href=True):
            href = link.get('href')
            text = link.get_text().strip()

            for pattern in patterns:
                if pattern.match(text) or pattern.match(href):
                    full_url = href if href.startswith('http') else self.base_url + href
                    if full_url not in subwoofer_links:
                        subwoofer_links.append(full_url)
                        print(f"Trouvé: {text} -> {full_url}")

        return subwoofer_links

    def scrape_subwoofer_list(self, url: str) -> List[Dict]:
        """Scrape une page de liste de subwoofers"""
        soup = self.get_page(url)
        if not soup:
            return []

        subwoofers = []

        # Chercher les tables de données
        tables = soup.find_all('table')
        for table in tables:
            rows = table.find_all('tr')

            # Essayer de trouver les en-têtes
            headers = []
            for th in rows[0].find_all(['th', 'td']) if rows else []:
                headers.append(th.get_text().strip())

            # Parser les données
            for row in rows[1:]:
                cells = row.find_all(['td', 'th'])
                if len(cells) > 0:
                    data = {}
                    for i, cell in enumerate(cells):
                        header = headers[i] if i < len(headers) else f'column_{i}'
                        data[header] = cell.get_text().strip()

                        # Chercher les liens vers les détails
                        link = cell.find('a', href=True)
                        if link:
                            data['detail_url'] = link['href'] if link['href'].startswith('http') else self.base_url + link['href']

                    # Vérifier si c'est un subwoofer en vente
                    # (On filtrera plus tard si on trouve l'information de disponibilité)
                    if data:
                        subwoofers.append(data)

        # Si pas de table, chercher d'autres structures
        if not subwoofers:
            # Chercher des divs ou autres conteneurs
            products = soup.find_all(['div', 'article'], class_=re.compile(r'.*(product|item|speaker).*', re.IGNORECASE))
            for product in products:
                data = {}

                # Extraire le titre/nom
                title = product.find(['h1', 'h2', 'h3', 'h4', 'a'])
                if title:
                    data['name'] = title.get_text().strip()
                    link = title.find('a', href=True) if title.name != 'a' else title
                    if link and link.get('href'):
                        data['detail_url'] = link['href'] if link['href'].startswith('http') else self.base_url + link['href']

                # Extraire d'autres informations
                for key in ['price', 'manufacturer', 'model', 'status']:
                    elem = product.find(class_=re.compile(f'.*{key}.*', re.IGNORECASE))
                    if elem:
                        data[key] = elem.get_text().strip()

                if data:
                    subwoofers.append(data)

        return subwoofers

    def scrape_subwoofer_details(self, url: str) -> Dict:
        """Scrape les détails d'un subwoofer spécifique"""
        soup = self.get_page(url)
        if not soup:
            return {}

        details = {'url': url}

        # Extraire le titre
        title = soup.find(['h1', 'h2'])
        if title:
            details['name'] = title.get_text().strip()

        # Chercher les spécifications dans les tables
        tables = soup.find_all('table')
        for table in tables:
            rows = table.find_all('tr')
            for row in rows:
                cells = row.find_all(['td', 'th'])
                if len(cells) >= 2:
                    key = cells[0].get_text().strip()
                    value = cells[1].get_text().strip()
                    details[key] = value

        # Chercher les informations de disponibilité
        availability_keywords = ['available', 'in stock', 'en vente', 'disponible', 'discontinued']
        for keyword in availability_keywords:
            elem = soup.find(text=re.compile(f'.*{keyword}.*', re.IGNORECASE))
            if elem:
                details['availability_info'] = elem.strip()
                break

        return details

    def scrape_all_subwoofers(self) -> List[Dict]:
        """Scrape tous les subwoofers en vente"""
        all_subwoofers = []

        print("Recherche des pages de subwoofers...")
        subwoofer_pages = self.find_subwoofer_pages()

        if not subwoofer_pages:
            print("Tentative de scraping direct des URLs connues...")
            # URLs communes pour les bases de données de haut-parleurs
            subwoofer_pages = [
                f"{self.base_url}/Subwoofers",
                f"{self.base_url}/subwoofers",
                f"{self.base_url}/woofers",
                f"{self.base_url}/database/subwoofers",
                f"{self.base_url}/speakers/subwoofers"
            ]

        for page_url in subwoofer_pages:
            print(f"\nScraping de {page_url}...")
            subwoofers = self.scrape_subwoofer_list(page_url)

            # Pour chaque subwoofer trouvé, récupérer les détails
            for sub in subwoofers:
                if 'detail_url' in sub:
                    print(f"  Récupération des détails: {sub.get('name', 'Unknown')}")
                    details = self.scrape_subwoofer_details(sub['detail_url'])
                    sub.update(details)
                    time.sleep(1)  # Pause pour ne pas surcharger le serveur

                all_subwoofers.append(sub)

            time.sleep(2)  # Pause entre les pages

        return all_subwoofers

    def filter_available_subwoofers(self, subwoofers: List[Dict]) -> List[Dict]:
        """Filtre pour ne garder que les subwoofers encore en vente"""
        available = []
        discontinued_keywords = ['discontinued', 'arrêté', 'obsolete', 'not available']

        for sub in subwoofers:
            # Vérifier si le subwoofer est marqué comme discontinued
            is_discontinued = False

            for key, value in sub.items():
                if isinstance(value, str):
                    value_lower = value.lower()
                    if any(keyword in value_lower for keyword in discontinued_keywords):
                        is_discontinued = True
                        break

            if not is_discontinued:
                available.append(sub)

        return available

    def save_to_json(self, data: List[Dict], filename: str = 'subwoofers_data.json'):
        """Sauvegarde les données dans un fichier JSON"""
        with open(filename, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=2, ensure_ascii=False)
        print(f"\nDonnées sauvegardées dans {filename}")

    def save_to_csv(self, data: List[Dict], filename: str = 'subwoofers_data.csv'):
        """Sauvegarde les données dans un fichier CSV"""
        if not data:
            print("Aucune donnée à sauvegarder")
            return

        import csv

        # Récupérer toutes les clés possibles
        all_keys = set()
        for item in data:
            all_keys.update(item.keys())

        with open(filename, 'w', newline='', encoding='utf-8') as f:
            writer = csv.DictWriter(f, fieldnames=sorted(all_keys))
            writer.writeheader()
            writer.writerows(data)

        print(f"Données sauvegardées dans {filename}")

def main():
    print("=" * 60)
    print("Scraper de caissons de basses - loudspeakerdatabase.com")
    print("=" * 60)

    scraper = SubwooferScraper()

    # Scraper tous les subwoofers
    print("\nDémarrage du scraping...")
    all_subwoofers = scraper.scrape_all_subwoofers()

    print(f"\n{len(all_subwoofers)} subwoofers trouvés au total")

    # Filtrer pour ne garder que ceux en vente
    available_subwoofers = scraper.filter_available_subwoofers(all_subwoofers)

    print(f"{len(available_subwoofers)} subwoofers encore en vente")

    # Sauvegarder les résultats
    if available_subwoofers:
        scraper.save_to_json(available_subwoofers, 'subwoofers_available.json')
        scraper.save_to_csv(available_subwoofers, 'subwoofers_available.csv')

        # Afficher un échantillon
        print("\nÉchantillon des données (premiers 3 subwoofers):")
        for i, sub in enumerate(available_subwoofers[:3], 1):
            print(f"\n--- Subwoofer {i} ---")
            for key, value in sub.items():
                print(f"  {key}: {value}")
    else:
        print("\nAucun subwoofer trouvé. Le site a peut-être changé de structure.")
        print("Sauvegarde de tous les résultats pour analyse...")
        scraper.save_to_json(all_subwoofers, 'subwoofers_all.json')

    print("\n" + "=" * 60)
    print("Scraping terminé!")
    print("=" * 60)

if __name__ == "__main__":
    main()
