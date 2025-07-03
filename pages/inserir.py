import streamlit as st
import re
import subprocess

def validar_texto(texto):
    """Valida se o texto contém apenas letras e espaços"""
    return bool(re.match("^[a-zA-ZÀ-ÿ ]+$", texto))

def calcular_mascara_tags(tags_selecionadas, todas_tags):
    """Converte tags selecionadas em máscara de bits"""
    mascara = 0
    for tag in tags_selecionadas:
        if tag in todas_tags:
            mascara |= 1 << todas_tags.index(tag)
    return mascara

st.title("🎵 Inserir Nova Música no Banco de Dados")

# Formulário de inserção
with st.form("form_inserir_musica", clear_on_submit=True):
    st.subheader("Informações Básicas")
    
    # Campos de texto
    nome_musica = st.text_input("Nome da Música*", max_chars=100).strip()
    nome_artista = st.text_input("Nome do Artista*", max_chars=100).strip()
    nome_album = st.text_input("Nome do Álbum", max_chars=100).strip()
    
    # Divisor visual
    st.markdown("---")
    st.subheader("Seleção de Tags")
    
    # Categorias de tags (deve bater com a estrutura do seu programa C++)
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
            "Psychedelic Rock", "Nu Metal", "Emo Pop", "Ska Punk", "Blue-eyed Soul"
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
            "Órgão", "Teclado", "Piano", "Sintetizador", "Acordeão"
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
            "50s e antes", "60s", "70s", "80s", "90s", "2000s", "2010s", "2020s"
        ]
    }
    
    # Checkboxes para cada categoria
    tags_selecionadas = {categoria: [] for categoria in CATEGORIAS}
    for categoria, tags in CATEGORIAS.items():
        with st.expander(f"🎵 {categoria}" if categoria == "Gêneros" else 
                        f"🥁 {categoria}" if categoria == "Instrumentos" else 
                        f"🎨 {categoria}"):
            for tag in tags:
                if st.checkbox(tag, key=f"inserir_{categoria}_{tag}"):
                    tags_selecionadas[categoria].append(tag)
    
    # Botão de submissão
    submitted = st.form_submit_button("Inserir Música")
    
    if submitted:
        # Validação dos campos
        erros = []
        if not nome_musica:
            erros.append("Nome da música é obrigatório")
        if not nome_artista:
            erros.append("Nome do artista é obrigatório")
        
        for campo, valor in [("Música", nome_musica), ("Artista", nome_artista), ("Álbum", nome_album)]:
            if valor and not validar_texto(valor):
                erros.append(f"{campo} contém caracteres inválidos (use apenas letras e espaços)")
        
        if erros:
            for erro in erros:
                st.error(erro)
        else:
            try:
                # Calcular máscaras de bits para cada categoria
                mask_generos = calcular_mascara_tags(tags_selecionadas["Gêneros"], CATEGORIAS["Gêneros"])
                mask_instrumentos = calcular_mascara_tags(tags_selecionadas["Instrumentos"], CATEGORIAS["Instrumentos"])
                mask_variedades = calcular_mascara_tags(tags_selecionadas["Variedades"], CATEGORIAS["Variedades"])
                
                # Chamar o programa auto
                args = [
                    "./auto", "inserir",
                    nome_musica,
                    nome_artista,
                    nome_album if nome_album else "",  # Envia vazio se não tiver álbum
                    str(mask_generos),
                    str(mask_instrumentos),
                    str(mask_variedades)
                ]
                
                # Executar o comando
                resultado = subprocess.run(args, check=True, capture_output=True, text=True)
                
                if resultado.returncode == 0:
                    st.success("✅ Música inserida com sucesso!")
                    st.code(f"Comando executado:\n{' '.join(args)}")
                    st.code(f"Saída do programa:\n{resultado.stdout}")
                else:
                    st.error(f"Erro ao inserir música:\n{resultado.stderr}")
            
            except subprocess.CalledProcessError as e:
                st.error(f"Falha ao executar o programa auto:\n{e.stderr}")
            except Exception as e:
                st.error(f"Erro inesperado: {str(e)}")  # Corrigido aqui - f-string terminada corretamente
