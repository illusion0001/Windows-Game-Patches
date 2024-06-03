#define STARTUP_META_DATA "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" \
"<CDataFileMgr__ContentsOfDataFileXml>\n" \
" <disabledFiles />\n" \
" <includedXmlFiles itemType=\"CDataFileMgr__DataFileArray\" />\n" \
" <includedDataFiles />\n" \
" <dataFiles itemType=\"CDataFileMgr__DataFile\">\n" \
"  <Item>\n" \
"   <filename>platform:/data/cdimages/scaleform_platform_pc.rpf</filename>\n" \
"   <fileType>RPF_FILE</fileType>\n" \
"  </Item>\n" \
"  <Item>\n" \
"   <filename>platform:/data/ui/value_conversion.rpf</filename>\n" \
"   <fileType>RPF_FILE</fileType>\n" \
"  </Item>\n" \
"  <Item>\n" \
"   <filename>platform:/data/ui/widgets.rpf</filename>\n" \
"   <fileType>RPF_FILE</fileType>\n" \
"  </Item>\n" \
"  <Item>\n" \
"   <filename>platform:/textures/ui/ui_photo_stickers.rpf</filename>\n" \
"   <fileType>RPF_FILE</fileType>\n" \
"  </Item>\n" \
"  <Item>\n" \
"   <filename>platform:/textures/ui/ui_platform.rpf</filename>\n" \
"   <fileType>RPF_FILE</fileType>\n" \
"  </Item>\n" \
"  <Item>\n" \
"   <filename>platform:/data/ui/stylesCatalog</filename>\n" \
"   <fileType>aWeaponizeDisputants</fileType>\n" \
"  </Item>\n" \
"  <Item>\n" \
"   <filename>platform:/data/cdimages/scaleform_frontend.rpf</filename>\n" \
"   <fileType>RPF_FILE_PRE_INSTALL</fileType>\n" \
"  </Item>\n" \
"  <Item>\n" \
"   <filename>platform:/textures/ui/ui_startup_textures.rpf</filename>\n" \
"   <fileType>RPF_FILE</fileType>\n" \
"  </Item>\n" \
"  <Item>\n" \
"   <filename>platform:/data/ui/startup_data.rpf</filename>\n" \
"   <fileType>RPF_FILE</fileType>\n" \
"  </Item>\n" \
"  <Item>\n" \
"    <filename>platform:/main_and_legal_menu_skip_data/legal_screen_patch.ymt</filename>\n" \
"    <fileType>STREAMING_FILE</fileType>\n" \
"    <registerAs>boot_flow/boot_launcher_flow</registerAs>\n" \
"    <overlay value=\"false\"/>\n" \
"    <patchFile value=\"false\"/>\n" \
"  </Item>\n" \
"  <Item>\n" \
"    <filename>platform:/main_and_legal_menu_skip_data/main_menu_patch.ymt</filename>\n" \
"    <fileType>STREAMING_FILE</fileType>\n" \
"    <registerAs>landing_page/flow/landing_launcher_flow</registerAs>\n" \
"    <overlay value=\"false\"/>\n" \
"    <patchFile value=\"false\"/>\n" \
"  </Item>\n" \
" </dataFiles>\n" \
" <contentChangeSets itemType=\"CDataFileMgr__ContentChangeSet\" />\n" \
" <patchFiles />\n" \
"</CDataFileMgr__ContentsOfDataFileXml>\n"

#define LEGAL_YMT_DATA "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" \
"<rage__fwuiFlowBlock>\n" \
" <ID>boot_flow</ID>\n" \
" <EntryPoints>\n" \
"  <Item>\n" \
"   <ID>default_entry</ID>\n" \
"   <Target>wait_for_profile</Target>\n" \
"  </Item>\n" \
"  <Item>\n" \
"   <ID>sign_out</ID>\n" \
"   <Target>wait_for_profile</Target>\n" \
"  </Item>\n" \
" </EntryPoints>\n" \
" <ExitPoints>\n" \
"  <Item>\n" \
"   <ID>exit</ID>\n" \
"  </Item>\n" \
" </ExitPoints>\n" \
" <FlowRoot>\n" \
"  <ID>input_context_switch</ID>\n" \
"  <State type=\"StateSetInputContext\">\n" \
"   <ContextType>BOOT_FLOW</ContextType>\n" \
"  </State>\n" \
"  <Children>\n" \
"   <Item>\n" \
"        <ID>wait_for_profile</ID>\n" \
"        <State type=\"StateWaitForProfileLoad\" />\n" \
"        <LinkMap>\n" \
"         <Item key=\"next\">\n" \
"          <Target>exit</Target>\n" \
"          <LinkInfo>LINK_TO_EXTERNAL</LinkInfo>\n" \
"         </Item>\n" \
"         <Item key=\"exit\">\n" \
"          <Target>exit</Target>\n" \
"          <LinkInfo>LINK_TO_EXTERNAL</LinkInfo>\n" \
"         </Item>\n" \
"         <Item key=\"yes\">\n" \
"          <Target>exit</Target>\n" \
"          <LinkInfo>LINK_TO_EXTERNAL</LinkInfo>\n" \
"         </Item>\n" \
"        </LinkMap>\n" \
"       </Item>\n" \
"  </Children>\n" \
" </FlowRoot>\n" \
"</rage__fwuiFlowBlock>\n"

#define MAIN_MENU_DATA "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n" \
"<rage__fwuiFlowBlock>\n" \
"  <ID>landing_flow</ID>\n" \
"  <EntryPoints>\n" \
"    <Item>\n" \
"      <Target>input_context_switch.set_feedback_context.launch_sp</Target>\n" \
"      <ID>sp</ID>\n" \
"    </Item>\n" \
"    <Item>\n" \
"      <Target>input_context_switch.set_feedback_context.launch_sp</Target>\n" \
"      <ID>mp</ID>\n" \
"    </Item>\n" \
"    <Item>\n" \
"      <Target>input_context_switch.set_feedback_context.boot_flow_activity_sentinel.boot_flow_startup</Target>\n" \
"      <ID>startup</ID>\n" \
"    </Item>\n" \
"    <Item>\n" \
"      <Target>input_context_switch.set_feedback_context.boot_flow_activity_sentinel.boot_flow_startup</Target>\n" \
"      <ID>sign_out</ID>\n" \
"    </Item>\n" \
"    <Item>\n" \
"      <Target>input_context_switch.set_feedback_context.launch_sp</Target>\n" \
"      <ID>mp_direct</ID>\n" \
"    </Item>\n" \
"  </EntryPoints>\n" \
"  <ExitPoints>\n" \
"    <Item>\n" \
"      <ID>exit</ID>\n" \
"    </Item>\n" \
"  </ExitPoints>\n" \
"  <FlowRoot>\n" \
"    <ID>landing_activity_sentinel</ID>\n" \
"    <State type=\"rage__StateActivitySentinel\">\n" \
"      <ActivityID>landing_main</ActivityID>\n" \
"    </State>\n" \
"    <Children>\n" \
"      <Item>\n" \
"        <ID>input_context_switch</ID>\n" \
"        <State type=\"StateSetInputContext\">\n" \
"          <ContextType>LANDING_PAGE</ContextType>\n" \
"        </State>\n" \
"        <Children>\n" \
"          <Item>\n" \
"            <ID>set_feedback_context</ID>\n" \
"            <State type=\"StateSetFeedbackContext\">\n" \
"              <ContextType>LANDING_PAGE</ContextType>\n" \
"            </State>\n" \
"            <Children>\n" \
"              <Item>\n" \
"                <ID>boot_flow_activity_sentinel</ID>\n" \
"                <State type=\"rage__StateActivitySentinel\">\n" \
"                  <ActivityID>boot_flow</ActivityID>\n" \
"                </State>\n" \
"                <Children>\n" \
"                  <Item>\n" \
"                    <ID>boot_flow_startup</ID>\n" \
"                    <State type=\"rage__StateAppLauncher\">\n" \
"                      <AppID>boot_flow</AppID>\n" \
"                      <EntryPoint>default_entry</EntryPoint>\n" \
"                    </State>\n" \
"                    <LinkMap>\n" \
"                      <Item key=\"exit\">\n" \
"                        <Target>^.^.launch_sp</Target>\n" \
"                      </Item>\n" \
"                    </LinkMap>\n" \
"                  </Item>\n" \
"                </Children>\n" \
"              </Item>\n" \
"              <Item>\n" \
"                <ID>launch_sp</ID>\n" \
"                <State type=\"StateDispatchScriptEvent\">\n" \
"                  <Channel>LANDING_PAGE</Channel>\n" \
"                  <Event>\n" \
"                    <Type>UISCRIPTEVENTTYPE_ITEM_SELECTED</Type>\n" \
"                    <HashParameter>LOAD_SP</HashParameter>\n" \
"                  </Event>\n" \
"                </State>\n" \
"                <LinkMap>\n" \
"                  <Item key=\"next\">\n" \
"                    <Target>exit</Target>\n" \
"                    <LinkInfo>LINK_TO_EXTERNAL</LinkInfo>\n" \
"                  </Item>\n" \
"                </LinkMap>\n" \
"              </Item>\n" \
"            </Children>\n" \
"          </Item>\n" \
"        </Children>\n" \
"      </Item>\n" \
"    </Children>\n" \
"  </FlowRoot>\n" \
"</rage__fwuiFlowBlock>\n"
