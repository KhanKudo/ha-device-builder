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
 *  time?: boolean
 *  ota_update?: boolean
 *  identifiers: string | string[]
 *  features?: {
 *      class: "binary_sensor" |
 *             "button" |
 *             "device_trigger" |
 *             "light" |
 *             "lock" |
 *             "number" |
 *             "scene" |
 *             "select" |
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
 *      max_kelvin?: number
 *      min_kelvin?: number
 *      color_temp_kelvin?: boolean
 *      flash_time_long?: number
 *      flash_time_short?: number
 *      resolution?: number
 *      min?: number
 *      max?: number
 *      step?: number
 *      options?: string[]
 *      type?: 'button_short_press' | 'button_short_release' | 'button_long_press' | 'button_long_release' | 'button_double_press' | 'button_triple_press' | 'button_quadruple_press' | 'button_quintuple_press' | string
 *      subtype?: 'turn_on' | 'turn_off' | 'button_1' | 'button_2' | 'button_3' | 'button_4' | 'button_5' | 'button_6' | string
 *      mode?: "onoff" |
 *                   "brightness" |
 *                   "color_temp" |
 *                   "hs" |
 *                   "xy" |
 *                   "rgb" |
 *                   "rgbw" |
 *                   "rgbww"
 * }[]
 * }}
//  *  "-WIP-alarm_control_panel" |
//  *  "binary_sensor" |
//  *  "button" |
//  *  "-WIP-camera" |
//  *  "-WIP-cover" |
//  *  "-WIP-device_tracker" |
//  *  "device_trigger" |
//  *  "-WIP-fan" |
//  *  "-WIP-humidifier" |
//  *  "-WIP-climate" |
//  *  "light" |
//  *  "lock" |
//  *  "number" |
//  *  "scene" |
//  *  "select" |
//  *  "sensor" |
//  *  "switch" |
//  *  "-WIP-tag_scanner" |
//  *  "-WIP-vacuum"
 */
const device = yaml.load(fs.readFileSync(haDeviceFilePath, 'utf8'))

function toCodeName(name) {
    return name.toLowerCase().replace(/ /g, '-').replace(/[^a-z0-9-]/g, '')
}

//TODO ----- add handlers -----
//TODO under yaml option maybe called native_handling:
//TODO      for selected feature classes, allow setting a pin number for light to control PWM brightness
//TODO          or maybe just on-off, in the future also RGB with option to select chip like WS2811 or WS2812
//TODO      for switch, maybe also allow a pin, simple on off toggle
//TODO this would then all in their respective on... listener functions allow a call to be made such as light1.preventDefault() or event.preventDefault()
//TODO      this would then skip any native handling that would happen by default, after the listener was called
//TODO a general native_handling-toggle should also exist, such as light1.disableNativeHandler(), with an enable of course too
//TODO the compiler should also know that, if no yaml options for native handling are present, then those functions should be either

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
 *      topic?: string
 *      max_kelvin?: number
 *      min_kelvin?: number
 *      color_temp_kelvin?: boolean
 *      flash_time_long?: number
 *      flash_time_short?: number
 *      min?: number
 *      max?: number
 *      step?: number
 *      brightness?: boolean
 *      brightness_scale?: number
 *      mode?: boolean
 *      supported_color_modes?: ("color_temp" | "hs" | "xy" | "rgb" | "rgbw" | "rgbww")[]
 *      options?: string[]
 *      schema?: 'json'
 *      automation_type?: 'trigger'
 *      type?: 'button_short_press' | 'button_short_release' | 'button_long_press' | 'button_long_release' | 'button_double_press' | 'button_triple_press' | 'button_quadruple_press' | 'button_quintuple_press' | string
 *      subtype?: 'turn_on' | 'turn_off' | 'button_1' | 'button_2' | 'button_3' | 'button_4' | 'button_5' | 'button_6' | string
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

if (device.time === true) { // default: false
    outputHeader += '#define TIME\n'
}
if (device.ota_update !== false) { // default: true
    outputHeader += '#define OTA_UPDATE\n'
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
    // replace AVAILABILITY_TOPIC
    .replace(/AVAILABILITY_TOPIC/g, availabilityTopic ?? '')
    // replace CODE_NAME
    .replace(/CODE_NAME/g, toCodeName(device.name))
    // replace DEVICE_ID
    .replace(/DEVICE_ID/g, Array.isArray(device.identifiers) ? device.identifiers.join('') : device.identifiers)
    // replace NAME
    .replace(/NAME/g, device.name)

outputHeader += '\n\n'

/**
 *
 * @param {{
 *      class: "binary_sensor" |
 *             "button" |
 *             "device_trigger" |
 *             "light" |
 *             "lock" |
 *             "number" |
 *             "scene" |
 *             "select" |
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
 *      max_kelvin?: number
 *      min_kelvin?: number
 *      color_temp_kelvin?: boolean
 *      flash_time_long?: number
 *      flash_time_short?: number
 *      resolution?: number
 *      min?: number
 *      max?: number
 *      step?: number
 *      options?: string[]
 *      type?: 'button_short_press' | 'button_short_release' | 'button_long_press' | 'button_long_release' | 'button_double_press' | 'button_triple_press' | 'button_quadruple_press' | 'button_quintuple_press' | string
 *      subtype?: 'turn_on' | 'turn_off' | 'button_1' | 'button_2' | 'button_3' | 'button_4' | 'button_5' | 'button_6' | string
 *      mode?: "onoff" |
 *                   "brightness" |
 *                   "color_temp" |
 *                   "hs" |
 *                   "xy" |
 *                   "rgb" |
 *                   "rgbw" |
 *                   "rgbww"
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
 *      topic?: string
 *      flash_time_long?: number
 *      flash_time_short?: number
 *      min?: number
 *      max?: number
 *      step?: number
 *      brightness?: boolean
 *      brightness_scale?: number
 *      mode?: boolean
 *      supported_color_modes?: ("color_temp" | "hs" | "xy" | "rgb" | "rgbw" | "rgbww")[]
 *      max_kelvin?: number
 *      min_kelvin?: number
 *      color_temp_kelvin?: boolean
 *      options?: string[]
 *      schema?: 'json'
 *      automation_type?: 'trigger'
 *      type?: 'button_short_press' | 'button_short_release' | 'button_long_press' | 'button_long_release' | 'button_double_press' | 'button_triple_press' | 'button_quadruple_press' | 'button_quintuple_press' | string
 *      subtype?: 'turn_on' | 'turn_off' | 'button_1' | 'button_2' | 'button_3' | 'button_4' | 'button_5' | 'button_6' | string
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
    /**
     * @type {string}
     */
    let component = components[feature.class]

    // process if conditions

    /**
     * @type {string[]}
     */
    let componentLines = component.split('\n')

    /**
     * @type {{condition: string, result: boolean}[]}
     */
    let conditionResultList = []

    componentLines.filter(line => line.includes('// start-if ')).forEach(line => {
        /**
         * @type {boolean}
         */
        let conditionResult

        const condition = line.slice(line.indexOf('// start-if ') + '// start-if '.length).trimEnd()

        switch (condition) {
            case 'brightness_supported':
                conditionResult = jsonFeature.brightness ?? false
                break
            case 'color_temp_supported':
                conditionResult = feature.mode === 'color_temp'
                break
            case 'effects_supported':
                conditionResult = feature.effect_list !== undefined && feature.effect_list.length > 0
                break
            case 'rgbww_supported':
                conditionResult = feature.mode === 'rgbww'
                break
            case 'rgbw_supported':
                conditionResult = feature.mode === 'rgbw'
                break
            case 'hs_supported':
                conditionResult = feature.mode === 'hs'
                break
            case 'xy_supported':
                conditionResult = feature.mode === 'xy'
                break
            case 'rgb_supported':
                conditionResult = feature.mode === 'rgb'
                break
            default:
                throw new Error(`start-if condition invalid, "${condition}"`)
        }

        conditionResultList.push({ condition, result: conditionResult })
    })

    for (const { condition, result } of conditionResultList) {
        const startIndex = componentLines.findIndex(line => line.includes(`// start-if ${condition}`))
        if (startIndex === -1) continue
        const endIndex = componentLines.findIndex(line => line.includes(`// end-if ${condition}`))
        if (endIndex === -1) throw new Error(`incomplete if statement, missing // end-if ${condition}`)

        if (result) {
            componentLines.splice(endIndex, 1)
            componentLines.splice(startIndex, 1)
        }
        else {
            componentLines.splice(startIndex, endIndex - startIndex + 1)
        }
    }

    component = componentLines.join('\n')

    // remove the part before the start identifier
    component = component.slice(component.indexOf(startIdentifier) + startIdentifier.length)
        // uncomment all "// uncomment:..." commands
        .replace(/\/\/ uncomment:/g, '')
        // insert effect-list-enum
        .replace('// __insert-effect-list-enum\r\n', (feature.effect_list?.length ?? 0) > 0 ? feature.effect_list.map(effect => effect.replace(/ /g, '_').replace(/[^a-zA-Z0-9_]/g, '')).join(',\n\t\t') + '\n' : '')
        // insert option-list-enum
        .replace('// __insert-option-list-enum\r\n', (feature.options?.length ?? 0) > 0 ? feature.options.map(effect => effect.replace(/ /g, '_').replace(/[^a-zA-Z0-9_]/g, '')).join(',\n\t\t') + '\n' : '')
        // insert effect-list
        .replace('// __insert-effect-list\r\n', (feature.effect_list?.length ?? 0) > 0 ? feature.effect_list.map(effect => `"${effect}"`).join(',\n\t\t') + '\n' : '')
        // insert option-list
        .replace('// __insert-option-list\r\n', (feature.options?.length ?? 0) > 0 ? feature.options.map(effect => `"${effect}"`).join(',\n\t\t') + '\n' : '')
        // replace UINT_RESOLUTION_T
        .replace(/UINT_RESOLUTION_T/g, `uint${2 ** Math.ceil(Math.log2(feature.resolution ?? 8))}_t`)
        // replace NUMBER_OF_EFFECTS
        .replace(/NUMBER_OF_EFFECTS/g, feature.effect_list?.length ?? 0)
        // replace NUMBER_OF_OPTIONS
        .replace(/NUMBER_OF_OPTIONS/g, feature.options?.length ?? 0)
        // replace COMMAND_TOPIC
        .replace(/COMMAND_TOPIC/g, jsonFeature.command_topic?.replace('~', jsonFeature['~']) ?? '')
        // replace STATE_TOPIC
        .replace(/STATE_TOPIC/g, jsonFeature.state_topic?.replace('~', jsonFeature['~']) ?? '')
        // replace MIN_KELVIN
        .replace(/MIN_KELVIN/g, jsonFeature.min_kelvin)
        // replace MAX_KELVIN
        .replace(/MAX_KELVIN/g, jsonFeature.max_kelvin)
        // replace RESOLUTION
        .replace(/RESOLUTION/g, feature.resolution ?? 8)
        // replace VAR_NAME
        .replace(/VAR_NAME/g, feature.var_name ?? toCodeName(jsonFeature.name).replace(/-/g, '_'))
        // replace RETAIN
        .replace(/RETAIN/g, jsonFeature.retain)
        // replace TOPIC
        .replace(/TOPIC/g, jsonFeature.topic?.replace('~', jsonFeature['~']) ?? '')
        // replace NAME
        .replace(/NAME/g, jsonFeature.name)
        // replace STEP
        .replace(/STEP/g, jsonFeature.step)
        // replace MIN
        .replace(/MIN/g, jsonFeature.min)
        // replace MAX
        .replace(/MAX/g, jsonFeature.max)
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
        case 'device_trigger':
            haMqtt.topic = `~/topic`
            haMqtt.automation_type = 'trigger'
            haMqtt.retain = false
            haMqtt.type = feature.type
            haMqtt.subtype = feature.subtype
            break
        case 'light':
            haMqtt.command_topic = `~/command`
            haMqtt.state_topic = `~/state`
            haMqtt.schema = 'json'
            haMqtt.brightness = feature.mode !== undefined && feature.mode !== 'onoff'
            haMqtt.supported_color_modes = [feature.mode ?? 'onoff']
            haMqtt.max_kelvin = feature.max_kelvin ?? 6500
            haMqtt.min_kelvin = feature.min_kelvin ?? 2700
            haMqtt.color_temp_kelvin = true
            haMqtt.brightness_scale = 2 ** (feature.resolution ?? 8) - 1
            haMqtt.effect_list = feature.effect_list
            haMqtt.effect = feature.effect_list !== undefined && feature.effect_list.length > 0
            break
        case 'lock':
            haMqtt.command_topic = `~/command`
            haMqtt.state_topic = `~/state`
            break
        case 'number':
            haMqtt.command_topic = `~/command`
            haMqtt.state_topic = `~/state`
            haMqtt.min = feature.min
            haMqtt.max = feature.max
            haMqtt.step = feature.step
            haMqtt.unit_of_measurement = feature.unit_of_measurement
            break
        case 'scene':
            haMqtt.command_topic = `~/command`
            break
        case 'select':
            haMqtt.command_topic = `~/command`
            haMqtt.state_topic = `~/state`
            haMqtt.options = feature.options
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
        default:
            throw new Error(`No feature class called "${feature.class}" exists`)
    }

    outputHeader += processFeature(feature, haMqtt)
    outputHeader += '\n'
})

// insert discovery publish
outputHeader = outputHeader.replace('// __insert-discovery-publish\r\n', haMqttJsonFeatures.map(jsonFeature => `publish("${jsonFeature['~']}/config", R"=-=-=(${JSON.stringify(jsonFeature)})=-=-=", true);`).join('\n\t\t') + '\n')

// write the output file
fs.writeFileSync('include/ha-device.h', outputHeader)

const libDeps = [
    'knolleary/PubSubClient@^2.8',
    'links2004/WebSockets@^2.4.1',
]

if (device.model !== 'esp32')
    libDeps.push('Hash')
if (device.time === true)
    libDeps.push('ropg/ezTime@^0.8.3')
if (device.features.some(feature => feature.class === 'light'))
    libDeps.push('bblanchon/ArduinoJson@^6.21.3')

// add the ArduinoOTA upload port to platformio.ini, if not already present
if (fs.existsSync('platformio.ini')) {
    let isModified = false

    let iniFile = fs.readFileSync('platformio.ini').toString()

    const libDepsRegex = /(?:lib_deps[ \t]*=[ \t]*)((?:(?:[ \t]*\r?\n[ \t]+)?[ \S]+)+)/

    if (iniFile.includes('lib_deps')) {
        const libs = iniFile.match(libDepsRegex)?.[1] ?? ''

        /** @type {Map<string, string>} */
        const versions = new Map()

        libDeps.push(...libs.replace(/[\r\t]/g, '').split('\n').map(lib => lib.trim()).filter(lib => lib !== ''))

        libDeps.forEach(lib => {
            if (!lib.includes('@'))
                lib += '@'
            const [name, version] = lib.split('@')
            if (!versions.has(name) || parseInt(versions.get(name).replace(/\D/g, '')) < parseInt(version.replace(/\D/g, ''))) {
                versions.set(name, version)
            }
        })

        libDeps.length = 0
        versions.forEach((version, name) => version === '' ? libDeps.push(name) : libDeps.push(`${name}@${version}`))
        libDeps.sort()

        const newIniFile = iniFile.replace(new RegExp(libDepsRegex, 'g'), `lib_deps =\n\t${libDeps.join('\n\t')}`)

        if (newIniFile !== iniFile) {
            isModified = true
            iniFile = newIniFile

            if (!iniFile.endsWith('\n'))
                iniFile += '\n'
        }
    }
    else {
        libDeps.sort()
        if (!iniFile.endsWith('\n'))
            iniFile += '\n'
        iniFile += `lib_deps =\n\t${libDeps.join('\n\t')}\n`
        isModified = true
    }

    let fileWasWritten = false

    const host = `${toCodeName(device.name)}.local`
    // check if hostname is active, otherwise don't add it, may be the initial upload, so the device has a blank project, no ArduinoOTA
    dns.lookup(host, 4, (err, address) => {
        if (err) return

        // if (fileWasWritten) return
        fileWasWritten = true

        // if there is no upload_port, then just append one to the end of the file
        if (!iniFile.includes('upload_port = ')) {
            if (!iniFile.endsWith('\n'))
                iniFile += '\n'
            iniFile += `upload_port = ${host}\n`

            isModified = true
        }
        // if there is an upload_port already present, then check if it's the same as the currectly suggested one, if so, don't do anything, otherwise overwrite it
        else if (!iniFile.includes(`upload_port = ${host}`)) {
            iniFile = iniFile.replace(/upload_port\s?=\s?.+\n/g, `upload_port = ${host}\n`)
            isModified = true
        }

        if (isModified === true)
            fs.writeFileSync('platformio.ini', iniFile)

    })

    setTimeout(() => {
        if (fileWasWritten) return
        fileWasWritten = true

        if (isModified === true)
            fs.writeFileSync('platformio.ini', iniFile)
    }, 1000)
}

// add the default gitlab auto-update pipeline config
if (fs.existsSync('.git/config')) {
    const gitConfig = fs.readFileSync('.git/config').toString()
    if (gitConfig.includes('https://gitlab.example.com/mqtt-users/')) {
        fs.copyFileSync(`${__dirname}/gitlab-ci-template.yml`, '.gitlab-ci.yml')
    }
}