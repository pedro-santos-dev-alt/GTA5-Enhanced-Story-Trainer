# GTA V Enhanced Story Trainer

Trainer para o **modo História do GTA V Enhanced**. O repositório mantém três variantes: o plugin `.asi` legado (ScriptHookV), um overlay ReShade apenas de diagnóstico e um plugin experimental para RAGE Plugin Hook (RPH).

> **Estado: EM DESENVOLVIMENTO.** A instalação atual ainda não confirmou o carregamento do plugin RPH nem o funcionamento das ações dentro do jogo. O último log do RPH revelou que não encontrou o DLL no caminho configurado. Foi colocada uma cópia no diretório esperado, mas essa correção ainda precisa de um novo arranque do jogo para validação. Não considerar esta versão pronta.

## Estado do overlay ReShade

O add-on ReShade é somente visual/diagnóstico: os seus botões não alteram o jogo. O artefacto **GTA5EnhancedStoryTrainer-RPH** é a implementação experimental prevista para ações reais, através da API de scripting do RAGE Plugin Hook. O funcionamento ainda não foi confirmado nesta instalação.

Na instalação diagnosticada em 20-10-2026, o jogo é `1.0.1158.16` e o ScriptHookV disponível é `1.0.1158.13`. A página do RPH/LSPDFR Enhanced Preview lista suporte ao jogo `1158.16`. O plugin novo foi criado para o RPH; a experiência em jogo ainda precisa de confirmação na máquina do utilizador.

Consulta [docs/INDEPENDENT_MODE.md](docs/INDEPENDENT_MODE.md) para a diferença entre ReShade e RPH e os passos de instalação.

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

## Requisitos do overlay ReShade

- GTA V Enhanced para Windows, iniciado no modo História.
- ReShade **com suporte completo a add-ons**. O ecrã de ReShade enviado pelo utilizador mostra o separador Add-ons e a versão 6.8.0.
- Add-on x64 `GTA5EnhancedStoryTrainerIndependent.addon64`, colocado junto ao ReShade na pasta do jogo.
- Abre o ReShade com `Home` e escolhe o add-on **GTA V Story Trainer**. O add-on regista `GTA5EnhancedStoryTrainerIndependent.log` na pasta do jogo.

O add-on visual não depende do ScriptHookV nem de um ASI loader. Continua a depender do ReShade como anfitrião gráfico e de interface.

## Modo funcional com RAGE Plugin Hook

- GTA V Enhanced no modo História e uma distribuição atual do **RAGE Plugin Hook compatível com a build instalada**. A compatibilidade é determinada pelo RPH, não pelo número de versão do plugin.
- O artefacto `GTA5EnhancedStoryTrainer-RPH` do GitHub Actions. Não inclui nem redistribui `RagePluginHook.dll`.
- Copia `GTA5EnhancedStoryTrainer.RPH.dll` para `Plugins` na pasta do jogo. Inicia o jogo através do RPH. A distribuição RPH instalada documenta `F4` como tecla predefinida da consola; executa `LoadPlugin "GTA5EnhancedStoryTrainer.RPH.dll"` se o `startup.rphs` não o carregar automaticamente.
- O menu abre quando o plugin é carregado. `F10` abre/fecha, setas navegam/alteram, `Enter` executa e `Backspace` volta/fecha.
- O plugin regista ações em `GTA5EnhancedStoryTrainer-RPH.log`.

Se `F4` abrir a consola do RPH e suspender temporariamente o jogo, isso é a tecla da consola, não o menu do trainer. O menu usa `F10`. Na instalação local diagnosticada em 20-10-2026, a consola foi remapeada para `F9` em `RagePluginHook.ini`. Confirma que `startup.rphs` contém `LoadPlugin "GTA5EnhancedStoryTrainer.RPH.dll"` e que o DLL está em `Plugins`. O add-on ReShade é apenas visual/diagnóstico e as suas opções não alteram o jogo. Mais detalhes em [docs/TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md).
O plugin inclui ações reais de jogador, armas, veículos e mundo através da API do RPH. Usa apenas no modo História; o plugin recusa executar ações quando deteta uma sessão online. Não uses o trainer ou o RPH em GTA Online.

## Requisitos do modo legado

- GTA V Enhanced para Windows, iniciado no modo História.
- Uma versão atual do ScriptHookV compatível com a versão instalada do jogo.
- ASI loader. A pasta de jogo usada para preparar este projeto já contém ScriptHookV e o ASI loader Enhanced.
- Visual Studio 2022 Build Tools com a carga de trabalho **Desktop development with C++**, se quiseres compilar localmente.

O plugin legado verifica se o `ScriptHookV.dll` corresponde ao build do jogo e carrega-o da pasta do jogo se ainda não estiver carregado. O DLL tem de estar ao lado de `GTA5_Enhanced.exe`.

## Build

Abre `GTA5EnhancedStoryTrainer.vcxproj` no Visual Studio e compila `Release | x64` para criar o `.asi` legado. Para o add-on independente, compila `GTA5EnhancedStoryTrainerIndependent.vcxproj`; o ReShade SDK 6.8.0 e o submódulo Dear ImGui têm de estar em `deps/reshade`.

O GitHub Actions obtém o ReShade SDK 6.8.0, compila as variantes C++, e compila o plugin RPH com o compilador .NET Framework 4.7.2 usando o SDK RPH apenas como referência. Os artefactos `.asi`, `.addon64` e `.RPH.dll` são separados. O DLL do anfitrião RPH não é incluído no artefacto do trainer.

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
- **Quero que as opções alterem o jogo:** usa o artefacto RPH. O add-on ReShade continua a ser apenas diagnóstico; não apresenta as suas opções como funcionais.
- **O plugin RPH não carrega:** confirma se instalaste uma versão RPH que declara suporte à build do jogo, se o DLL está em `Plugins`, se o nome passado a `LoadPlugin` está correto e consulta `RagePluginHook.log` e `GTA5EnhancedStoryTrainer-RPH.log`.

Para pedir ajuda, fecha o jogo e partilha `GTA5EnhancedStoryTrainer.log` e `asiloader.log`. Remove nomes de utilizador ou outros dados pessoais dos caminhos antes de os publicar.

## Safety and limitations

- Apenas modo História/offline. Não uses no GTA Online nem no FiveM. O plugin desativa o menu durante uma sessão online como proteção adicional.
- O add-on ReShade é diagnóstico e não executa ações. O plugin RPH permanece experimental e depende do RPH, da compatibilidade com a build instalada e de validação prática no jogo.
- O `.asi` legado só funciona com ScriptHookV compatível com a versão exata do jogo.
- O projeto não altera ficheiros originais do GTA; chama natives do ScriptHookV durante a execução.
- O registo pode não ser criado se o ASI não carregar ou se o script não arrancar; nesse caso, começa por consultar `asiloader.log`.

## License

MIT. See [LICENSE](LICENSE).
