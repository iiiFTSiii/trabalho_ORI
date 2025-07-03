import streamlit as st
import subprocess

st.title("🗑️ Excluir Música")

# Campo numérico
numero = st.number_input("Digite o número do registro a excluir:", min_value=0, step=1)

# Botão para executar
if st.button("Excluir"):
    try:
        args = ["./auto", "excluir", str(numero)]
        resultado = subprocess.run(
            args,
            capture_output=True,
            text=True,
            check=True
        )
        st.success("✅ Exclusão realizada com sucesso!")
        st.code(resultado.stdout)
    except subprocess.CalledProcessError as e:
        st.error("❌ Erro ao executar o programa:")
        st.code(e.stderr)
