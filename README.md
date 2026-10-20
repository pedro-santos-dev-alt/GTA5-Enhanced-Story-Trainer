# GTA V Enhanced Story Trainer

Trainer para o **modo História/offline do GTA V Enhanced**, com duas implementações separadas: o plugin `.asi` legado, que depende do ScriptHookV, e o novo add-on visual independente do ScriptHookV, que usa o ReShade.

## Estado da versão independente

A versão independente abre uma interface de diagnóstico dentro do menu de add-ons do ReShade e deteta a versão do executável do jogo. **As ações de jogo estão deliberadamente bloqueadas:** o add-on ainda não tem um adaptador nativo validado para GTA V Enhanced. O ReShade desenha a interface, mas não fornece acesso às funções internas do GTA necessárias para spawnar veículos, dar armas ou alterar o jogador.

Na instalação diagnosticada em 20-10-2026, o jogo é `1.0.1158.16` e o ScriptHookV disponível é `1.0.1158.13`. O ASI loader carregava o trainer, mas o ScriptHookV não estava carregado no processo. Por isso, o código legado nunca registava o script que desenha o menu ou lê o F10. A nova interface separa o desenho do menu do acesso ao estado do jogo e não tenta carregar ou chamar ScriptHookV.

Consulta [docs/INDEPENDENT_MODE.md](docs/INDEPENDENT_MODE.md) para instalação, arquitetura, limitações e atualização por versão do jogo.

## Implementação legada com ScriptHookV

O `.asi` legado mantém as funções de trainer descritas abaixo, mas só pode funcionar quando houver um ScriptHookV que suporte exatamente o build instalado do jogo.

## Features in this first release

- **Jogador:** invencibilidade, recuperar vida, colete a 100, limpar nível de procurado e impedir que a polícia atribua estrelas.
- **Armas:** escolher e receber armas individuais, carregar um conjunto completo, munição infinita no carregador e remover todas as armas.
- **Veículos:** spawn de uma seleção de carros, motas, aeronaves, veículos de emergência e especiais; reparar e limpar; aplicar melhoramentos; escolher uma peça por slot; personalizar cores primária e secundária em RGB; pintura aleatória; matrícula `STORY`; invencibilidade.
- **Mundo:** escolher meteorologia, remover meteorologia persistente, definir a hora e parar/retomar o relógio.
- O menu bloqueia as ações quando deteta uma sessão online.

## Controls

| Key | Action |
| --- | --- |
| `F10` | Abrir/fechar o menu |
| `Setas cima/baixo` | Percorrer opções |
| `Setas esquerda/direita` | Alterar veículo, arma, meteorologia, hora, slot de mod ou valores RGB |
| `Enter` | Executar/aplicar a opção selecionada |
| `Backspace` | Voltar à página principal ou fechar |

O menu abre automaticamente quando o script do trainer arranca. Usa `F10` para fechar ou voltar a abrir. `Backspace` volta à página principal e fecha o menu quando já estás nessa página. O atalho foi mudado de `F7` para evitar conflito com a tecla de troca de personagem configurada no jogo.

## Requisitos do modo independente

- GTA V Enhanced para Windows, iniciado no modo História.
- ReShade **com suporte completo a add-ons**. O ecrã de ReShade enviado pelo utilizador mostra o separador Add-ons e a versão 6.8.0.
- Add-on x64 `GTA5EnhancedStoryTrainerIndependent.addon64`, colocado junto ao ReShade na pasta do jogo.
- Abre o ReShade com `Home` e escolhe o add-on **GTA V Story Trainer**. O add-on regista `GTA5EnhancedStoryTrainerIndependent.log` na pasta do jogo.

O add-on visual não depende do ScriptHookV nem de um ASI loader. Continua a depender do ReShade como anfitrião gráfico e de interface.

## Requisitos do modo legado

- GTA V Enhanced para Windows, iniciado no modo História.
- Uma versão atual do ScriptHookV compatível com a versão instalada do jogo.
- ASI loader. A pasta de jogo usada para preparar este projeto já contém ScriptHookV e o ASI loader Enhanced.
- Visual Studio 2022 Build Tools com a carga de trabalho **Desktop development with C++**, se quiseres compilar localmente.

O plugin legado verifica se o `ScriptHookV.dll` corresponde ao build do jogo e carrega-o da pasta do jogo se ainda não estiver carregado. O DLL tem de estar ao lado de `GTA5_Enhanced.exe`.

## Build

Abre `GTA5EnhancedStoryTrainer.vcxproj` no Visual Studio e compila `Release | x64` para criar o `.asi` legado. Para o add-on independente, compila `GTA5EnhancedStoryTrainerIndependent.vcxproj`; o ReShade SDK 6.8.0 e o submódulo Dear ImGui têm de estar em `deps/reshade`.

O GitHub Actions obtém o ReShade SDK 6.8.0, compila ambas as variantes x64 e publica os artefactos `.asi` e `.addon64` separadamente. O repositório não contém ficheiros do jogo nem binários de terceiros.

## Install

1. Fecha o jogo.
2. Para instalar a interface independente, descarrega o artefacto `GTA5EnhancedStoryTrainer-Independent` e copia `GTA5EnhancedStoryTrainerIndependent.addon64` para a pasta onde está o ReShade e `GTA5_Enhanced.exe`.
3. Mantém o `.asi` legado apenas se também quiseres usar ScriptHookV compatível. Para desativar o legado, fecha o jogo e muda a extensão de `GTA5EnhancedStoryTrainer.asi` para `.disabled`.
4. Inicia o jogo no modo História, abre o overlay do ReShade com `Home` e escolhe **GTA V Story Trainer** na área de add-ons.
5. Consulta `GTA5EnhancedStoryTrainerIndependent.log` para confirmar deteção da build e registo do menu.

Para remover o add-on independente, fecha o jogo e apaga `GTA5EnhancedStoryTrainerIndependent.addon64`.

## Logs e resolução de problemas

O trainer legado cria `GTA5EnhancedStoryTrainer.log` ao lado do `.asi`. O add-on independente cria `GTA5EnhancedStoryTrainerIndependent.log` e regista inicialização, versão detetada, abertura do overlay e encerramento. Não regista cada frame.

- **Não existe `GTA5EnhancedStoryTrainer.asi` no `asiloader.log`:** o ASI loader não encontrou o trainer. Confirma que o `.asi` foi extraído e copiado para a mesma pasta de `GTA5_Enhanced.exe`.
- **O `asiloader.log` lista o `.asi` como falhado:** confirma que o ficheiro não está bloqueado pelo Windows e que corresponde à arquitetura x64. A versão atual deixa de rejeitar o plugin quando o ScriptHookV ainda não arrancou; atualiza o `.asi` e consulta o log próprio.
- **O `.asi` carrega mas não existe `GTA5EnhancedStoryTrainer.log`:** o plugin não conseguiu iniciar o logger ou a thread de arranque. Consulta `asiloader.log`; se existir, confirma permissões de escrita na pasta do jogo.
- **O log indica versões `INCOMPATIBLE`:** instala a versão oficial do ScriptHookV que corresponda ao build mostrado do GTA V Enhanced. O trainer evita carregar um hook desatualizado.
- **O log indica que não conseguiu carregar `ScriptHookV.dll`:** confirma se o DLL existe ao lado de `GTA5_Enhanced.exe`, se é a edição compatível com GTA V Enhanced e se o Windows não o bloqueou. O log inclui o código de erro do Windows.
- **O log indica que falta um export:** confirma se tens a edição atual do ScriptHookV compatível com a versão instalada do GTA V Enhanced.
- **O log mostra `Trainer script registered` mas não mostra `ScriptHookV started the trainer script`:** o registo foi aceite, mas o ScriptHookV ainda não executou o script; fecha o jogo e revê a compatibilidade/versão do hook.
- **O log mostra `Menu auto-opened` ou `F10 detected: menu opened`, mas o menu não aparece:** o script arrancou; consulta as mensagens seguintes e testa sem outros overlays/mods gráficos para isolar um conflito de renderização.
- **O log não mostra a linha de F10:** confirma que a janela do jogo tem foco e volta a testar com a tecla `F10` num teclado físico.
- **O add-on independente não aparece:** confirma que a instalação do ReShade tem suporte completo a add-ons, que o ficheiro `.addon64` está na pasta do ReShade e consulta `ReShade.log`.
- **O menu aparece mas as ações estão indisponíveis:** é o comportamento esperado até ser implementado e validado um adaptador de jogo para essa build. Atualizar a interface gráfica, por si só, não cria acesso às funções internas do GTA.

Para pedir ajuda, fecha o jogo e partilha `GTA5EnhancedStoryTrainer.log` e `asiloader.log`. Remove nomes de utilizador ou outros dados pessoais dos caminhos antes de os publicar.

## Safety and limitations

- Apenas modo História/offline. Não uses no GTA Online nem no FiveM. O plugin desativa o menu durante uma sessão online como proteção adicional.
- A interface independente não executa ações no jogo. Um adaptador para o motor do GTA precisa de engenharia reversa e testes por build; não se ativa automaticamente em builds desconhecidos.
- O `.asi` legado só funciona com ScriptHookV compatível com a versão exata do jogo.
- O projeto não altera ficheiros originais do GTA; chama natives do ScriptHookV durante a execução.
- O registo pode não ser criado se o ASI não carregar ou se o script não arrancar; nesse caso, começa por consultar `asiloader.log`.

## License

MIT. See [LICENSE](LICENSE).
