import streamlit as st
import subprocess
from typing import List, Dict

st.title("Busca e Seleção de Tags Musicais")

# --- Configuração inicial ---
if 'busca' not in st.session_state:
    st.session_state.busca = ""

# --- Barra de pesquisa ---
def update_busca():
    st.session_state.busca = st.session_state.autocomplete_input

busca = st.text_input(
    "🔍 Pesquisar:", 
    value=st.session_state.busca, 
    key="autocomplete_input",
    on_change=update_busca
)

# --- Dados das categorias ---
CATEGORIAS = {
    "Gêneros": [
        "Pop", "Rock", "Hip Hop", "R&B", "Jazz", "Blues", "Country", "EDM",
        "Reggae", "Classical", "Metal", "Punk", "Funk", "Soul", "Disco", "House",
        "Techno", "Trance", "Dubstep", "Gospel", "Latin", "K-Pop", "J-Pop",
        "Afrobeat", "Dancehall", "Trap", "Ska", "Bluegrass", "Folk", "Ambient",
        "Synthwave", "Industrial", "Indie Rock", "Alternative Rock",
        "Progressive Rock", "Hard Rock", "Grunge", "Emo", "Pop Punk",
        "Hardcore Punk", "Post-Punk", "New Wave", "Alternative", "Lo-fi",
        "Chillout", "Drum and Bass", "Acid Jazz", "Latin Pop", "Mariachi",
        "Salsa", "Bachata", "Tango", "Flamenco", "Bossa Nova", "Neo-Soul",
        "Trip Hop", "Contemporary R&B", "Christian Rock", "Gothic Rock",
        "Psychedelic Rock", "Nu Metal", "Emo Pop", "Ska Punk", "outro"
    ],
    "Instrumentos": [
        "Cordas", "Violão", "Violoncelo", "Violino", "Viola", "Viola caipira",
        "Cavaquinho", "Ukulele", "Guitarra", "Contrabaixo acústico",
        "Baixo elétrico", "Bandolim", "Banjo",
        "Percussão", "Bateria", "Cajon", "Pandeiro", "Cuica", "Cowbell",
        "Chocalho", "Triângulo", "Recoreco", "Beatbox", "Percussão digital",
        "Zabumba",
        "Saxofone", "Flauta", "Clarinete",
        "Trombone", "Trompete", "Trompa", "Corneta", "Tuba", "Sousafone",
        "Órgão", "Teclado", "Piano", "Sintetizador", "outro"
    ],
    "Variedades": [
        "Vocal Masculino", "Vocal Feminino", "Sem vocal", "Chorus",
        "Gang vocals", "Quartet vocals", "Harmonias vocais", "Dueto", "Trio",
        "Low range", "Mid range", "High range", "Acapella",
        "Fast pace", "Slow pace",
        "Calma", "Intensa",
        "Carregada", "Minimalista",
        "Banda", "Artista solo", "Dupla", "Quarteto", "Big band",
        "Orquestra",
        "Feliz", "Triste", "Épica", "Odiosa", "Romântica", "Comédia",
        "Crítica", "Ambiente",
        "50s e antes", "60s", "70s", "80s", "90s", "2000s", "2010s", "2020s", "outro"
    ]
}

# --- Gerenciamento das seleções ---
selected_items = {categoria: [] for categoria in CATEGORIAS}

for categoria, itens in CATEGORIAS.items():
    with st.expander(f"🎵 {categoria}" if categoria == "Gêneros" else 
                    f"🥁 {categoria}" if categoria == "Instrumentos" else 
                    f"🎨 {categoria}"):
        for i, item in enumerate(itens):
            if st.checkbox(item, key=f"{categoria}_{i}"):
                selected_items[categoria].append(i)

# --- Exibição dos resultados selecionados ---
st.markdown("---")
st.write("**🔎 Texto pesquisado:**", st.session_state.busca or "_(vazio)_")
for categoria in CATEGORIAS:
    selected_names = [CATEGORIAS[categoria][i] for i in selected_items[categoria]]
    st.write(f"**✔️ {categoria} escolhidos:**", ", ".join(selected_names) or "_nenhum_")

# --- Exibição dos resultados finais quando houver busca ---
if st.session_state.busca:
    linhas = []
    try:
        mask_generos = sum(1 << idx for idx in selected_items["Gêneros"])
        mask_instrumentos = sum(1 << idx for idx in selected_items["Instrumentos"])
        mask_variedades = sum(1 << idx for idx in selected_items["Variedades"])
        
        args = [
            "./auto", "buscar", st.session_state.busca,
            str(mask_generos), str(mask_instrumentos), str(mask_variedades)
        ]
        res = subprocess.run(args, capture_output=True, text=True, check=True)
        linhas = res.stdout.strip().splitlines()
    except subprocess.CalledProcessError as e:
        st.error(f"Erro ao executar ./auto:\n{e.stderr}")

    if linhas:
        st.markdown("## 🎶 Resultados encontrados")
        for i in range(0, len(linhas), 3):
            if i + 2 < len(linhas):
                musica = linhas[i]
                artista = linhas[i+1]
                album = linhas[i+2]
                st.markdown(f"**{musica}**")
                st.write(f"- Artista: {artista}")
                st.write(f"- Álbum: {album}")
                st.markdown("---")
