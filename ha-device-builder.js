const dns = require('dns')
const fs = require('fs')

const haDeviceFilePath = process.argv[2]

if (typeof haDeviceFilePath !== 'string' || !haDeviceFilePath.endsWith('.ha-device.yaml') || !fs.existsSync(haDeviceFilePath)) {
    throw new Error('First Parameter was not a valid path to a *.ha-device.yaml file')
}

const yaml = require('js-yaml')

/**
 * @type {{
 *  name: string
 *  manufacturer?: string
 *  model?: string
 *  suggested_area?: string
 *  sw_version?: string
 *  availability?: boolean
 *  retain?: boolean
 *  wan_deployment?: boolean
 *  identifiers: string | string[]
 *  features?: {
 *      class: "binary_sensor" |
 *             "button" |
 *             "light" |
 *             "sensor" |
 *             "switch"
 *      name: string
 *      unique_id: string
 *      retain?: boolean
 *      var_name?: string
 *      icon?: string
 *      expire_after?: number
 *      off_delay?: number
 *      force_update?: boolean
 *      unit_of_measurement?: string
 *      effect_list?: string | string[]
 *      brightness?: boolean
 *      temperature?: boolean
 *      rgb?: boolean
 *      max_mireds?: number
 *      min_mireds?: number
 * }[]
 * }}
//  *  "-WIP-alarm_control_panel" |
//  *  "binary_sensor" |
//  *  "button" |
//  *  "-WIP-camera" |
//  *  "-WIP-cover" |
//  *  "-WIP-device_tracker" |
//  *  "-WIP-device_trigger" |
//  *  "-WIP-fan" |
//  *  "-WIP-humidifier" |
//  *  "-WIP-climate" |
//  *  "light" |
//  *  "-WIP-lock" |
//  *  "-WIP-number" |
//  *  "-WIP-scene" |
//  *  "-WIP-select" |
//  *  "sensor" |
//  *  "switch" |
//  *  "-WIP-tag_scanner" |
//  *  "-WIP-vacuum"
 */
const device = yaml.load(fs.readFileSync(haDeviceFilePath, 'utf8'))

function toCodeName(name) {
    return name.toLowerCase().replace(/ /g, '-').replace(/[^a-z0-9-]/g, '')
}

/**
 * @type {[key: string]: string}
 */
const components = {}

for (const fileName of fs.readdirSync(`${__dirname}/components/`)) {
    components[fileName.slice(0, -2)] = fs.readFileSync(`${__dirname}/components/${fileName}`).toString()
}

const discoveryPrefix = 'homeassistant'
/**
 * @type {{
 *      name: string
 *      unique_id: string
 *      retain: boolean
 *      icon?: string
 *      expire_after?: number
 *      off_delay?: number
 *      force_update?: boolean
 *      unit_of_measurement?: string
 *      effect_list?: string | string[]
 *      availability_topic?: string
 *      state_topic?: string
 *      command_topic?: string
 *      brightness_command_topic?: string
 *      brightness_state_topic?: string
 *      color_temp_command_topic?: string
 *      color_temp_state_topic?: string
 *      rgb_command_topic?: string
 *      rgb_state_topic?: string
 *      effect_command_topic?: string
 *      effect_state_topic?: string
 *      max_mireds?: number
 *      min_mireds?: number
 *      device: {
 *          name: string
 *          model?: string
 *          manufacturer?: string
 *          suggested_area?: string
 *          sw_version?: string
 *          identifiers?: string | string[]
 *      }
 * }[]}
 */
const haMqttJsonFeatures = []

let outputHeader = ''

if (device.wan_deployment === true) {
    outputHeader += '#define WAN_DEPLOYMENT\n'
}

const startIdentifier = '// start\r\n'

/**
 * @type {string | null}
 */
const availabilityTopic = (device.availability !== false) ? `home/${toCodeName(device.name)}/availability` : null

// remove the part before the start identifier,
outputHeader += components['manager'].slice(components['manager'].indexOf(startIdentifier) + startIdentifier.length)
    // uncomment all "// uncomment:..." commands,
    .replace(/\/\/ uncomment:/g, '')
    // replace AVAILABILITY_TOPIC,
    .replace(/AVAILABILITY_TOPIC/g, availabilityTopic ?? '')
    // replace CODE_NAME,
    .replace(/CODE_NAME/g, toCodeName(device.name))
    // replace NAME,
    .replace(/NAME/g, device.name)

outputHeader += '\n\n'

/**
 *
 * @param {{
 *      class: "binary_sensor" |
 *             "button" |
 *             "light" |
 *             "sensor" |
 *             "switch"
 *      name: string
 *      unique_id: string
 *      retain: boolean
 *      var_name?: string
 *      icon?: string
 *      expire_after?: number
 *      off_delay?: number
 *      force_update?: boolean
 *      unit_of_measurement?: string
 *      effect_list?: string | string[]
 *      brightness?: boolean
 *      temperature?: boolean
 *      rgb?: boolean
 *      max_mireds?: number
 *      min_mireds?: number
 * }} feature
 * @param {{
 *      name: string
 *      unique_id: string
 *      icon?: string
 *      expire_after?: number
 *      off_delay?: number
 *      force_update?: boolean
 *      unit_of_measurement?: string
 *      effect_list?: string | string[]
 *      availability_topic?: string
 *      state_topic?: string
 *      command_topic?: string
 *      brightness_command_topic?: string
 *      brightness_state_topic?: string
 *      color_temp_command_topic?: string
 *      color_temp_state_topic?: string
 *      rgb_command_topic?: string
 *      rgb_state_topic?: string
 *      effect_command_topic?: string
 *      effect_state_topic?: string
 *      max_mireds?: number
 *      min_mireds?: number
 *      device: {
 *          name: string
 *          model?: string
 *          manufacturer?: string
 *          suggested_area?: string
 *          sw_version?: string
 *          identifiers: string | string[]
 *      }
 * }} jsonFeature
 *
 * @returns {string | null}
 */
function processFeature(feature, jsonFeature) {
    let component = components[feature.class]

    // remove the part before the start identifier,
    component = component.slice(component.indexOf(startIdentifier) + startIdentifier.length)
        // uncomment all "// uncomment:..." commands,
        .replace(/\/\/ uncomment:/g, '')
        // replace COMMAND_TOPIC,
        .replace(/COMMAND_TOPIC/g, jsonFeature.command_topic?.replace('~', jsonFeature['~']) ?? '')
        // replace STATE_TOPIC,
        .replace(/STATE_TOPIC/g, jsonFeature.state_topic?.replace('~', jsonFeature['~']) ?? '')
        // replace VAR_NAME,
        .replace(/VAR_NAME/g, feature.var_name ?? toCodeName(jsonFeature.name).replace(/-/g, '_'))
        // replace RETAIN,
        .replace(/RETAIN/g, jsonFeature.retain)
        // replace NAME,
        .replace(/NAME/g, jsonFeature.name)
        + '\n'

    return component
}

device.features.forEach((feature, index) => {
    haMqttJsonFeatures[index] = {}
    const haMqtt = haMqttJsonFeatures[index]

    haMqtt['~'] = `${discoveryPrefix}/${feature.class}/${feature.unique_id}`
    haMqtt.name = feature.name
    haMqtt.unique_id = feature.unique_id
    haMqtt.icon = feature.icon
    haMqtt.retain = feature.retain ?? device.retain ?? true
    haMqtt.device = {
        manufacturer: device.manufacturer,
        model: device.model,
        name: device.name,
        suggested_area: device.suggested_area,
        sw_version: device.sw_version,
        identifiers: device.identifiers
    }

    if (availabilityTopic !== null) {
        haMqtt.availability_topic = availabilityTopic
    }

    switch (feature.class) {
        case 'binary_sensor':
            haMqtt.expire_after = feature.expire_after
            haMqtt.force_update = feature.force_update
            haMqtt.off_delay = feature.off_delay
            haMqtt.state_topic = `~/state`
            break
        case 'button':
            haMqtt.command_topic = `~/command`
            haMqtt.retain = false
            break
        case 'light':
            haMqtt.command_topic = `~/command`
            haMqtt.state_topic = `~/state`
            if (feature.brightness) {
                haMqtt.brightness_command_topic = `~/brightness_command`
                haMqtt.brightness_state_topic = `~/brightness_state`
            }
            if (feature.temperature) {
                haMqtt.color_temp_command_topic = `~/temperature_command`
                haMqtt.color_temp_state_topic = `~/temperature_state`
                haMqtt.max_mireds = feature.max_mireds
                haMqtt.min_mireds = feature.min_mireds
            }
            if (feature.rgb) {
                haMqtt.rgb_command_topic = `~/rgb_command`
                haMqtt.rgb_state_topic = `~/rgb_state`
            }
            if (feature.effect_list) {
                haMqtt.effect_command_topic = `~/effect_command`
                haMqtt.effect_state_topic = `~/effect_state`
            }
            break
        case 'sensor':
            haMqtt.expire_after = feature.expire_after
            haMqtt.force_update = feature.force_update
            haMqtt.unit_of_measurement = feature.unit_of_measurement
            haMqtt.state_topic = `~/state`
            break
        case 'switch':
            haMqtt.command_topic = `~/command`
            haMqtt.state_topic = `~/state`
            break
        default: break
    }

    outputHeader += processFeature(feature, haMqtt)
    outputHeader += '\n'
})

// insert discovery publish
outputHeader = outputHeader.replace('// __insert-discovery-publish\r\n', haMqttJsonFeatures.map(jsonFeature => `client.publish("${jsonFeature['~']}/config", R"=-=-=(${JSON.stringify(jsonFeature)})=-=-=");`).join('\n\t\t') + '\n')

// write the output file
fs.writeFileSync('include/ha-device.h', outputHeader)

// add the ArduinoOTA upload port to platformio.ini, if not already present
if (fs.existsSync('platformio.ini')) {
    const iniFile = fs.readFileSync('platformio.ini').toString()
    const host = `${toCodeName(device.name)}.local`
    // check if hostname is active, otherwise don't add it, may be the initial upload, so the device has a blank project, no ArduinoOTA
    dns.lookup(host, 4, (err, address) => {
        if (err) return

        // if there is no upload_port, then just append one to the end of the file
        if (!iniFile.includes('upload_port = ')) {
            fs.appendFileSync('platformio.ini', `upload_port = ${host}\n`)
        }
        // if there is an upload_port already present, then check if it's the same as the currectly suggested one, if so, don't do anything, otherwise overwrite it
        else if (!iniFile.includes(`upload_port = ${host}`)) {
            fs.writeFileSync('platformio.ini', iniFile.split('\n').map(line => line.includes('upload_port = ') ? `upload_port = ${host}` : line).join('\n'))
        }
    })
}