import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID, CONF_TRIGGER_ID, CONF_FILE, CONF_DEVICE, CONF_VOLUME, CONF_MODE
from esphome.components import uart

DEPENDENCIES = ["uart"]
CODEOWNERS = ["@hn"]

dyplayer_ns = cg.esphome_ns.namespace("dyplayer")
DYPlayer = dyplayer_ns.class_("DYPlayer", cg.Component)
DYPlayerFinishedPlaybackTrigger = dyplayer_ns.class_(
    "DYPlayerFinishedPlaybackTrigger", automation.Trigger.template()
)
DYPlayerIsPlayingCondition = dyplayer_ns.class_(
    "DYPlayerIsPlayingCondition", automation.Condition
)

MULTI_CONF = True
CONF_FOLDER = "folder"
CONF_EQ_PRESET = "eq_preset"
CONF_ON_FINISHED_PLAYBACK = "on_finished_playback"

EqPreset = dyplayer_ns.enum("EqPreset")
EQ_PRESET = {
    "NORMAL": EqPreset.NORMAL,
    "POP": EqPreset.POP,
    "ROCK": EqPreset.ROCK,
    "JAZZ": EqPreset.JAZZ,
    "CLASSIC": EqPreset.CLASSIC,
}
PlayMode = dyplayer_ns.enum("PlayMode")
PLAY_MODE = {
    "REPEAT": PlayMode.REPEAT,
    "REPEATONE": PlayMode.REPEATONE,
    "ONEOFF": PlayMode.ONEOFF,
    "RANDOM": PlayMode.RANDOM,
    "REPEATDIR": PlayMode.REPEATDIR,
    "RANDOMDIR": PlayMode.RANDOMDIR,
    "SEQUENCEDIR": PlayMode.SEQUENCEDIR,
    "SEQUENCE": PlayMode.SEQUENCE,
}
Device = dyplayer_ns.enum("Device")
DEVICE = {
    "USB": Device.USB,
    "TF_CARD": Device.TF_CARD,
}

NextAction = dyplayer_ns.class_("NextAction", automation.Action)
PreviousAction = dyplayer_ns.class_("PreviousAction", automation.Action)
InterludeFileAction = dyplayer_ns.class_("InterludeFileAction", automation.Action)
PlayFileAction = dyplayer_ns.class_("PlayFileAction", automation.Action)
SelectFileAction = dyplayer_ns.class_("SelectFileAction", automation.Action)
PlayFolderAction = dyplayer_ns.class_("PlayFolderAction", automation.Action)
SetVolumeAction = dyplayer_ns.class_("SetVolumeAction", automation.Action)
VolumeUpAction = dyplayer_ns.class_("VolumeUpAction", automation.Action)
VolumeDownAction = dyplayer_ns.class_("VolumeDownAction", automation.Action)
SetModeAction = dyplayer_ns.class_("SetModeAction", automation.Action)
SetEqAction = dyplayer_ns.class_("SetEqAction", automation.Action)
SleepAction = dyplayer_ns.class_("SleepAction", automation.Action)
ResetAction = dyplayer_ns.class_("ResetAction", automation.Action)
StartAction = dyplayer_ns.class_("StartAction", automation.Action)
PauseAction = dyplayer_ns.class_("PauseAction", automation.Action)
StopAction = dyplayer_ns.class_("StopAction", automation.Action)
RandomAction = dyplayer_ns.class_("RandomAction", automation.Action)
SetDeviceAction = dyplayer_ns.class_("SetDeviceAction", automation.Action)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(DYPlayer),
            cv.Optional(CONF_ON_FINISHED_PLAYBACK): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(
                        DYPlayerFinishedPlaybackTrigger
                    ),
                }
            ),
        }
    ).extend(uart.UART_DEVICE_SCHEMA)
)
FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "dyplayer", baud_rate=9600, require_tx=True
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    for conf in config.get(CONF_ON_FINISHED_PLAYBACK, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)


@automation.register_action(
    "dyplayer.play_next",
    NextAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
        }
    ),
)
async def dyplayer_next_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var


@automation.register_action(
    "dyplayer.play_previous",
    PreviousAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
        }
    ),
)
async def dyplayer_previous_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var


@automation.register_action(
    "dyplayer.interlude_file",
    InterludeFileAction,
    cv.maybe_simple_value(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
            cv.Required(CONF_FILE): cv.templatable(cv.int_),
        },
        key=CONF_FILE,
    ),
)
async def dyplayer_interlude_file_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    template_ = await cg.templatable(config[CONF_FILE], args, float)
    cg.add(var.set_file(template_))
    return var


@automation.register_action(
    "dyplayer.play",
    PlayFileAction,
    cv.maybe_simple_value(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
            cv.Required(CONF_FILE): cv.templatable(cv.int_),
        },
        key=CONF_FILE,
    ),
)
async def dyplayer_play_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    template_ = await cg.templatable(config[CONF_FILE], args, float)
    cg.add(var.set_file(template_))
    return var


@automation.register_action(
    "dyplayer.select",
    SelectFileAction,
    cv.maybe_simple_value(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
            cv.Required(CONF_FILE): cv.templatable(cv.int_),
        },
        key=CONF_FILE,
    ),
)
async def dyplayer_select_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    template_ = await cg.templatable(config[CONF_FILE], args, float)
    cg.add(var.set_file(template_))
    return var


@automation.register_action(
    "dyplayer.play_folder",
    PlayFolderAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
            cv.Required(CONF_FOLDER): cv.templatable(cv.int_),
            cv.Optional(CONF_FILE): cv.templatable(cv.int_),
        }
    ),
)
async def dyplayer_play_folder_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    template_ = await cg.templatable(config[CONF_FOLDER], args, float)
    cg.add(var.set_folder(template_))
    if CONF_FILE in config:
        template_ = await cg.templatable(config[CONF_FILE], args, float)
        cg.add(var.set_file(template_))
    return var


@automation.register_action(
    "dyplayer.set_device",
    SetDeviceAction,
    cv.maybe_simple_value(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
            cv.Required(CONF_DEVICE): cv.enum(DEVICE, upper=True),
        },
        key=CONF_DEVICE,
    ),
)
async def dyplayer_set_device_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    template_ = await cg.templatable(config[CONF_DEVICE], args, Device)
    cg.add(var.set_device(template_))
    return var


@automation.register_action(
    "dyplayer.set_volume",
    SetVolumeAction,
    cv.maybe_simple_value(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
            cv.Required(CONF_VOLUME): cv.templatable(cv.int_),
        },
        key=CONF_VOLUME,
    ),
)
async def dyplayer_set_volume_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    template_ = await cg.templatable(config[CONF_VOLUME], args, float)
    cg.add(var.set_volume(template_))
    return var


@automation.register_action(
    "dyplayer.volume_up",
    VolumeUpAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
        }
    ),
)
async def dyplayer_volume_up_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var


@automation.register_action(
    "dyplayer.volume_down",
    VolumeDownAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
        }
    ),
)
async def dyplayer_volume_down_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var


@automation.register_action(
    "dyplayer.set_eq",
    SetEqAction,
    cv.maybe_simple_value(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
            cv.Required(CONF_EQ_PRESET): cv.templatable(cv.enum(EQ_PRESET, upper=True)),
        },
        key=CONF_EQ_PRESET,
    ),
)
async def dyplayer_set_eq_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    template_ = await cg.templatable(config[CONF_EQ_PRESET], args, EqPreset)
    cg.add(var.set_eq(template_))
    return var


@automation.register_action(
    "dyplayer.set_mode",
    SetModeAction,
    cv.maybe_simple_value(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
            cv.Required(CONF_MODE): cv.templatable(cv.enum(PLAY_MODE, upper=True)),
        },
        key=CONF_MODE,
    ),
)
async def dyplayer_set_mode_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    template_ = await cg.templatable(config[CONF_MODE], args, PlayMode)
    cg.add(var.set_mode(template_))
    return var


@automation.register_action(
    "dyplayer.sleep",
    SleepAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
        }
    ),
)
async def dyplayer_sleep_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var


@automation.register_action(
    "dyplayer.reset",
    ResetAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
        }
    ),
)
async def dyplayer_reset_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var


@automation.register_action(
    "dyplayer.start",
    StartAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
        }
    ),
)
async def dyplayer_start_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var


@automation.register_action(
    "dyplayer.pause",
    PauseAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
        }
    ),
)
async def dyplayer_pause_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var


@automation.register_action(
    "dyplayer.stop",
    StopAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
        }
    ),
)
async def dyplayer_stop_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var


@automation.register_action(
    "dyplayer.random",
    RandomAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
        }
    ),
)
async def dyplayer_random_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var


@automation.register_condition(
    "dyplayer.is_playing",
    DYPlayerIsPlayingCondition,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(DYPlayer),
        }
    ),
)
async def dyplayer_is_playing_to_code(config, condition_id, template_arg, args):
    var = cg.new_Pvariable(condition_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var
