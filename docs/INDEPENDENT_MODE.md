# Add-on ReShade e plugin RAGE Plugin Hook

## O que falhou na versão anterior

Os logs partilhados em 20-10-2026 mostravam que o ASI loader carregava `GTA5EnhancedStoryTrainer.asi`, mas o processo não tinha `ScriptHookV.dll` carregado. O trainer dependia dos exports do ScriptHookV para registar um script, consultar natives, ler as teclas e desenhar o menu. Sem essa inicialização, o código do menu nunca arrancava.

O ficheiro `ScriptHookV.dll` existia na pasta, mas a versão instalada era `1.0.1158.13` e o executável do jogo era `1.0.1158.16`. A versão do ScriptHookV disponível na página oficial declara suporte até à `1.0.1158.13`. Forçar o carregamento desse hook antigo não é uma correção segura: pode deixar os endereços e estruturas internas incompatíveis com o executável atual.

## ReShade: apenas diagnóstico

`GTA5EnhancedStoryTrainerIndependent.addon64` é um add-on x64 do ReShade. Usa a API oficial de add-ons do ReShade e Dear ImGui para mostrar uma interface de diagnóstico. Não carrega, importa nem procura `ScriptHookV.dll`; também não usa o ASI loader.

O add-on lê o recurso de versão do executável em execução, apresenta essa versão e escreve `GTA5EnhancedStoryTrainerIndependent.log`. As páginas Jogador, Armas, Veículos e Mundo listam a funcionalidade planeada, mas marcam cada ação como indisponível.

O utilizador identificou corretamente que a janela ReShade anterior tinha opções sem efeito no jogo. O add-on continua a ser apenas visual: não tem API de natives e não deve ser usado como o trainer funcional.

## Plugin funcional com RAGE Plugin Hook

Foi acrescentado `src/rph_plugin.cs`, um plugin .NET Framework que usa a API documentada do RPH para desenhar o menu dentro do jogo e chamar natives de gameplay. Inclui opções de invencibilidade, vida/colete, procurado, armas e munições, spawn de uma seleção de veículos, reparação/melhorias e meteorologia/hora.

A instalação local observada usa GTA Enhanced `1.0.1158.16`. A página oficial da distribuição Enhanced Preview do LSPDFR lista o RPH incluído como compatível com Enhanced `1158.16` (17 de setembro de 2026). O repositório não instala o RPH nem redistribui os seus binários. Obtém o RPH de uma fonte autorizada, verifica a compatibilidade com a build atual, coloca o trainer em `Plugins` e inicia o jogo pelo RPH. A distribuição RPH instalada documenta `F4` como tecla predefinida da consola; se o `startup.rphs` não carregar o trainer automaticamente, executa `LoadPlugin "GTA5EnhancedStoryTrainer.RPH.dll"`. A experiência deste plugin ainda não foi confirmada no jogo; ver [TROUBLESHOOTING.md](TROUBLESHOOTING.md).

O pacote NuGet `RagePluginHook` é usado como referência de compilação; o workflow remove `RagePluginHook.dll` do output antes de publicar o artefacto. A API do RPH permanece requisito de runtime.

## Limites e atualização

RPH é uma dependência de runtime separada; esse plugin não é um clone de ScriptHookV. A atualização de jogo pode tornar o próprio RPH incompatível. Atualiza o RPH para uma versão que declare suporte ao novo build, compila e experimenta o trainer apenas em História. A tabela abaixo não transforma a compatibilidade do host numa validação end-to-end de cada ação.

## Compatibilidade e atualizações

| Componente | Compatibilidade atual |
| --- | --- |
| Overlay ReShade | ReShade 6.8.0 com suporte completo a add-ons, x64; diagnóstico apenas |
| GTA V Enhanced 1.0.1158.16 | Build listada como suportada pela distribuição RPH incluída no LSPDFR Enhanced Preview 0.4.9 Build 9695 |
| RPH Story Trainer | Compilado contra API de referência RPH; requer RPH instalado em runtime; experiência deste plugin ainda por confirmar no jogo |
| ScriptHookV | Não utilizado pelo plugin RPH nem pelo add-on ReShade |

O build do jogo é obtido do recurso de versão do executável a cada arranque. Para acrescentar suporte de jogo no futuro:

1. Confirmar que a versão de RPH disponível declara suporte ao novo build do GTA Enhanced.
2. Atualizar a versão de referência do SDK no projeto e compilar o plugin.
3. Testar cada ação no modo História e conferir `GTA5EnhancedStoryTrainer-RPH.log`.
4. Atualizar a matriz de compatibilidade só depois dessa validação.

O repositório ainda não contém esses adaptadores. A tabela deverá ser atualizada apenas depois de validação real; não declarar compatibilidade só porque o projeto compila.

## Instalação

1. ReShade 6.8.0 ou compatível, instalado com **full add-on support**.
2. Descarregar o artefacto `GTA5EnhancedStoryTrainer-Independent` do GitHub Actions.
3. Copiar `GTA5EnhancedStoryTrainerIndependent.addon64` para a pasta de jogo onde está o ReShade.
4. Arrancar o GTA V Enhanced no modo História, abrir o ReShade com `Home` e selecionar `GTA V Story Trainer`.
5. Confirmar o carregamento em `ReShade.log` e a versão detetada em `GTA5EnhancedStoryTrainerIndependent.log`.

Para executar as ações do trainer, usa o plugin RPH descrito na secção anterior; não uses a janela ReShade para isso.

## Dependências de desenvolvimento

- Visual Studio 2022 Build Tools, carga de trabalho Desktop development with C++.
- ReShade SDK `v6.8.0` e o seu submódulo Dear ImGui. O workflow fixa esta versão para obter headers consistentes com o ReShade do utilizador.
- Windows SDK x64.

O workflow GitHub Actions descarrega o SDK oficial durante o build e publica o `.addon64` como artefacto separado. Não copia nem publica os binários do ReShade.

## Backup do mod legado

A versão ScriptHookV 0.4.1 foi preservada no computador onde este trabalho foi feito como `GTA5EnhancedStoryTrainer.asi.backup-independent-0.4.1`. O código fonte legado continua na raiz do repositório e a variante `.asi` continua a ser compilada separadamente.

## Fontes e licenças

- [ReShade API reference](https://github.com/crosire/reshade/blob/v6.8.0/REFERENCE.md)
- [Exemplo oficial de overlay do ReShade](https://github.com/crosire/reshade/blob/v6.8.0/examples/10-texture_overlay/texture_overlay_addon.cpp)
- [ReShade v6.8.0](https://github.com/crosire/reshade/tree/v6.8.0), código sob BSD-3-Clause; o Dear ImGui usado pelo SDK é MIT. O projeto consome os headers no build, sem redistribuir o SDK.
- [Documentação de criação de plugins RPH](https://docs.ragepluginhook.net/html/51ce7151-4a76-4b21-be90-34d114769311.htm)
- [API dinâmica de natives RPH](https://docs.ragepluginhook.net/html/T_Rage_Native_DynamicNativeFunction.htm)
- [RPH/LSPDFR Enhanced Preview: compatibilidade 1158.16](https://www.lcpdfr.com/downloads/gta5mods/g17media/52970-lspd-first-response-enhanced-preview/)
