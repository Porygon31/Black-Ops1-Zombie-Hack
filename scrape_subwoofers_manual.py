#!/usr/bin/env python3
"""
Script interactif pour extraire manuellement les données de subwoofers
depuis loudspeakerdatabase.com
"""

import json
import re
from typing import List, Dict
from bs4 import BeautifulSoup

def print_header():
    """Affiche l'en-tête"""
    print("=" * 70)
    print("  Extracteur Manuel de Subwoofers - loudspeakerdatabase.com")
    print("=" * 70)
    print()

def print_instructions():
    """Affiche les instructions"""
    print("📋 INSTRUCTIONS:")
    print()
    print("1. Ouvrez votre navigateur web")
    print("2. Allez sur https://loudspeakerdatabase.com")
    print("3. Naviguez vers la section des subwoofers")
    print("4. Options pour extraire les données:")
    print()
    print("   Option A - HTML Complet:")
    print("   - Faites clic droit → Inspecter (F12)")
    print("   - Clic droit sur <html> → Copy → Copy Outer HTML")
    print("   - Collez ici")
    print()
    print("   Option B - Table Spécifique:")
    print("   - Clic droit sur la table → Inspecter")
    print("   - Clic droit sur <table> → Copy → Copy Outer HTML")
    print("   - Collez ici")
    print()
    print("   Option C - Saisie Manuelle:")
    print("   - Copiez les données ligne par ligne")
    print()
    print("-" * 70)
    print()

def get_multiline_input(prompt: str) -> str:
    """Récupère une entrée multi-lignes"""
    print(prompt)
    print("(Tapez 'END' sur une ligne vide pour terminer)")
    print()

    lines = []
    while True:
        try:
            line = input()
            if line.strip().upper() == 'END':
                break
            lines.append(line)
        except EOFError:
            break

    return '\n'.join(lines)

def parse_html(html: str) -> List[Dict]:
    """Parse le HTML pour extraire les subwoofers"""
    soup = BeautifulSoup(html, 'html.parser')
    subwoofers = []

    # Chercher les tables
    tables = soup.find_all('table')
    print(f"\n✓ Trouvé {len(tables)} table(s)")

    for idx, table in enumerate(tables, 1):
        print(f"\n  Traitement de la table {idx}...")

        # Trouver les en-têtes
        headers = []
        header_row = table.find('thead') or table.find('tr')

        if header_row:
            for th in header_row.find_all(['th', 'td']):
                headers.append(th.get_text().strip())

        print(f"  En-têtes: {headers}")

        # Parser les lignes
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
                        data[f'{header}_url'] = link['href']

                if data:
                    subwoofers.append(data)

        print(f"  → {len(rows)} entrées extraites")

    # Si pas de table, chercher des divs/articles
    if not subwoofers:
        print("\n  Recherche d'autres structures...")
        products = soup.find_all(['div', 'article', 'li'])

        for elem in products[:50]:  # Limiter à 50 pour éviter trop de bruit
            # Chercher un titre
            title = elem.find(['h1', 'h2', 'h3', 'h4', 'a'])
            if title:
                text = title.get_text().strip()
                # Filtrer les titres qui semblent être des produits
                if len(text) > 3 and len(text) < 100:
                    data = {'name': text}

                    # Chercher un lien
                    link = title if title.name == 'a' else title.find('a')
                    if link and link.get('href'):
                        data['url'] = link['href']

                    # Extraire le texte complet
                    data['full_text'] = elem.get_text(separator=' ', strip=True)[:200]

                    subwoofers.append(data)

        print(f"  → {len(subwoofers)} produits potentiels trouvés")

    return subwoofers

def parse_manual_input(text: str) -> List[Dict]:
    """Parse une saisie manuelle"""
    subwoofers = []

    # Diviser par lignes
    lines = [line.strip() for line in text.split('\n') if line.strip()]

    print(f"\n✓ {len(lines)} lignes à traiter")

    for line in lines:
        # Essayer de détecter le format
        # Format: "Nom | Fabricant | Modèle | ..."
        if '|' in line or '\t' in line:
            separator = '|' if '|' in line else '\t'
            parts = [p.strip() for p in line.split(separator)]

            data = {}
            if len(parts) >= 1:
                data['name'] = parts[0]
            if len(parts) >= 2:
                data['manufacturer'] = parts[1]
            if len(parts) >= 3:
                data['model'] = parts[2]

            # Ajouter les autres colonnes
            for i in range(3, len(parts)):
                data[f'field_{i}'] = parts[i]

            subwoofers.append(data)
        else:
            # Une seule colonne, probablement le nom
            subwoofers.append({'name': line})

    return subwoofers

def filter_available(subwoofers: List[Dict]) -> List[Dict]:
    """Filtre les subwoofers en vente"""
    discontinued_keywords = [
        'discontinued', 'arrêté', 'obsolete', 'not available',
        'out of stock', 'épuisé', 'unavailable', 'retired'
    ]

    available = []
    for sub in subwoofers:
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

def save_results(subwoofers: List[Dict]):
    """Sauvegarde les résultats"""
    if not subwoofers:
        print("\n⚠️  Aucune donnée à sauvegarder")
        return

    # Sauvegarder en JSON
    with open('subwoofers_manual.json', 'w', encoding='utf-8') as f:
        json.dump(subwoofers, f, indent=2, ensure_ascii=False)
    print(f"\n✓ Données sauvegardées dans subwoofers_manual.json")

    # Sauvegarder en CSV
    import csv

    all_keys = set()
    for item in subwoofers:
        all_keys.update(item.keys())

    with open('subwoofers_manual.csv', 'w', newline='', encoding='utf-8') as f:
        writer = csv.DictWriter(f, fieldnames=sorted(all_keys))
        writer.writeheader()
        writer.writerows(subwoofers)

    print(f"✓ Données sauvegardées dans subwoofers_manual.csv")

def display_sample(subwoofers: List[Dict], count=3):
    """Affiche un échantillon"""
    print(f"\n📋 Échantillon ({min(count, len(subwoofers))} premiers):")
    for i, sub in enumerate(subwoofers[:count], 1):
        print(f"\n  [{i}] {sub.get('name', 'N/A')}")
        for key, value in list(sub.items())[:5]:
            if len(str(value)) < 100:
                print(f"      • {key}: {value}")

def main():
    print_header()
    print_instructions()

    while True:
        print("\n🔧 OPTIONS:")
        print("  1 - Coller du HTML")
        print("  2 - Saisie manuelle (format: Nom | Fabricant | Modèle)")
        print("  3 - Charger depuis un fichier HTML")
        print("  4 - Quitter")
        print()

        choice = input("Votre choix (1-4): ").strip()

        if choice == '1':
            html = get_multiline_input("\n📝 Collez le HTML ici:")
            if html:
                print("\n⏳ Analyse en cours...")
                subwoofers = parse_html(html)

                if subwoofers:
                    print(f"\n✅ {len(subwoofers)} subwoofers extraits")

                    # Filtrer
                    available = filter_available(subwoofers)
                    print(f"   {len(available)} encore en vente")

                    # Afficher échantillon
                    display_sample(available if available else subwoofers)

                    # Sauvegarder
                    save = input("\n💾 Sauvegarder les résultats? (o/n): ").strip().lower()
                    if save == 'o' or save == 'y':
                        save_results(available if available else subwoofers)
                        print("\n✅ Sauvegarde terminée!")
                else:
                    print("\n❌ Aucune donnée extraite. Vérifiez le HTML.")

        elif choice == '2':
            text = get_multiline_input("\n📝 Saisissez les données (format: Nom | Fabricant | Modèle):")
            if text:
                print("\n⏳ Analyse en cours...")
                subwoofers = parse_manual_input(text)

                if subwoofers:
                    print(f"\n✅ {len(subwoofers)} entrées créées")

                    # Filtrer
                    available = filter_available(subwoofers)

                    # Afficher échantillon
                    display_sample(available if available else subwoofers)

                    # Sauvegarder
                    save = input("\n💾 Sauvegarder les résultats? (o/n): ").strip().lower()
                    if save == 'o' or save == 'y':
                        save_results(available if available else subwoofers)
                        print("\n✅ Sauvegarde terminée!")
                else:
                    print("\n❌ Aucune donnée extraite.")

        elif choice == '3':
            filename = input("\n📂 Chemin du fichier HTML: ").strip()
            try:
                with open(filename, 'r', encoding='utf-8') as f:
                    html = f.read()

                print("\n⏳ Analyse en cours...")
                subwoofers = parse_html(html)

                if subwoofers:
                    print(f"\n✅ {len(subwoofers)} subwoofers extraits")

                    # Filtrer
                    available = filter_available(subwoofers)
                    print(f"   {len(available)} encore en vente")

                    # Afficher échantillon
                    display_sample(available if available else subwoofers)

                    # Sauvegarder
                    save = input("\n💾 Sauvegarder les résultats? (o/n): ").strip().lower()
                    if save == 'o' or save == 'y':
                        save_results(available if available else subwoofers)
                        print("\n✅ Sauvegarde terminée!")
                else:
                    print("\n❌ Aucune donnée extraite. Vérifiez le fichier.")

            except FileNotFoundError:
                print(f"\n❌ Fichier non trouvé: {filename}")
            except Exception as e:
                print(f"\n❌ Erreur: {e}")

        elif choice == '4':
            print("\n👋 Au revoir!")
            break

        else:
            print("\n❌ Choix invalide. Essayez encore.")

        print("\n" + "=" * 70)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\n👋 Interruption par l'utilisateur. Au revoir!")
