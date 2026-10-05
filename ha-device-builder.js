const dns = require('dns')
const fs = require('fs')

const haDeviceFilePath = process.argv[2]

if (typeof haDeviceFilePath !== 'string' || !haDeviceFilePath.endsWith('.ha-device.yaml') || !fs.existsSync(haDeviceFilePath)) {
    throw new Error('First Parameter was not a valid path to a *.ha-device.yaml file')
}

const MD5 = function (d) { var r = M(V(Y(X(d), 8 * d.length))); return r.toLowerCase() }; function M(d) { for (var _, m = "0123456789ABCDEF", f = "", r = 0; r < d.length; r++)_ = d.charCodeAt(r), f += m.charAt(_ >>> 4 & 15) + m.charAt(15 & _); return f } function X(d) { for (var _ = Array(d.length >> 2), m = 0; m < _.length; m++)_[m] = 0; for (m = 0; m < 8 * d.length; m += 8)_[m >> 5] |= (255 & d.charCodeAt(m / 8)) << m % 32; return _ } function V(d) { for (var _ = "", m = 0; m < 32 * d.length; m += 8)_ += String.fromCharCode(d[m >> 5] >>> m % 32 & 255); return _ } function Y(d, _) { d[_ >> 5] |= 128 << _ % 32, d[14 + (_ + 64 >>> 9 << 4)] = _; for (var m = 1732584193, f = -271733879, r = -1732584194, i = 271733878, n = 0; n < d.length; n += 16) { var h = m, t = f, g = r, e = i; f = md5_ii(f = md5_ii(f = md5_ii(f = md5_ii(f = md5_hh(f = md5_hh(f = md5_hh(f = md5_hh(f = md5_gg(f = md5_gg(f = md5_gg(f = md5_gg(f = md5_ff(f = md5_ff(f = md5_ff(f = md5_ff(f, r = md5_ff(r, i = md5_ff(i, m = md5_ff(m, f, r, i, d[n + 0], 7, -680876936), f, r, d[n + 1], 12, -389564586), m, f, d[n + 2], 17, 606105819), i, m, d[n + 3], 22, -1044525330), r = md5_ff(r, i = md5_ff(i, m = md5_ff(m, f, r, i, d[n + 4], 7, -176418897), f, r, d[n + 5], 12, 1200080426), m, f, d[n + 6], 17, -1473231341), i, m, d[n + 7], 22, -45705983), r = md5_ff(r, i = md5_ff(i, m = md5_ff(m, f, r, i, d[n + 8], 7, 1770035416), f, r, d[n + 9], 12, -1958414417), m, f, d[n + 10], 17, -42063), i, m, d[n + 11], 22, -1990404162), r = md5_ff(r, i = md5_ff(i, m = md5_ff(m, f, r, i, d[n + 12], 7, 1804603682), f, r, d[n + 13], 12, -40341101), m, f, d[n + 14], 17, -1502002290), i, m, d[n + 15], 22, 1236535329), r = md5_gg(r, i = md5_gg(i, m = md5_gg(m, f, r, i, d[n + 1], 5, -165796510), f, r, d[n + 6], 9, -1069501632), m, f, d[n + 11], 14, 643717713), i, m, d[n + 0], 20, -373897302), r = md5_gg(r, i = md5_gg(i, m = md5_gg(m, f, r, i, d[n + 5], 5, -701558691), f, r, d[n + 10], 9, 38016083), m, f, d[n + 15], 14, -660478335), i, m, d[n + 4], 20, -405537848), r = md5_gg(r, i = md5_gg(i, m = md5_gg(m, f, r, i, d[n + 9], 5, 568446438), f, r, d[n + 14], 9, -1019803690), m, f, d[n + 3], 14, -187363961), i, m, d[n + 8], 20, 1163531501), r = md5_gg(r, i = md5_gg(i, m = md5_gg(m, f, r, i, d[n + 13], 5, -1444681467), f, r, d[n + 2], 9, -51403784), m, f, d[n + 7], 14, 1735328473), i, m, d[n + 12], 20, -1926607734), r = md5_hh(r, i = md5_hh(i, m = md5_hh(m, f, r, i, d[n + 5], 4, -378558), f, r, d[n + 8], 11, -2022574463), m, f, d[n + 11], 16, 1839030562), i, m, d[n + 14], 23, -35309556), r = md5_hh(r, i = md5_hh(i, m = md5_hh(m, f, r, i, d[n + 1], 4, -1530992060), f, r, d[n + 4], 11, 1272893353), m, f, d[n + 7], 16, -155497632), i, m, d[n + 10], 23, -1094730640), r = md5_hh(r, i = md5_hh(i, m = md5_hh(m, f, r, i, d[n + 13], 4, 681279174), f, r, d[n + 0], 11, -358537222), m, f, d[n + 3], 16, -722521979), i, m, d[n + 6], 23, 76029189), r = md5_hh(r, i = md5_hh(i, m = md5_hh(m, f, r, i, d[n + 9], 4, -640364487), f, r, d[n + 12], 11, -421815835), m, f, d[n + 15], 16, 530742520), i, m, d[n + 2], 23, -995338651), r = md5_ii(r, i = md5_ii(i, m = md5_ii(m, f, r, i, d[n + 0], 6, -198630844), f, r, d[n + 7], 10, 1126891415), m, f, d[n + 14], 15, -1416354905), i, m, d[n + 5], 21, -57434055), r = md5_ii(r, i = md5_ii(i, m = md5_ii(m, f, r, i, d[n + 12], 6, 1700485571), f, r, d[n + 3], 10, -1894986606), m, f, d[n + 10], 15, -1051523), i, m, d[n + 1], 21, -2054922799), r = md5_ii(r, i = md5_ii(i, m = md5_ii(m, f, r, i, d[n + 8], 6, 1873313359), f, r, d[n + 15], 10, -30611744), m, f, d[n + 6], 15, -1560198380), i, m, d[n + 13], 21, 1309151649), r = md5_ii(r, i = md5_ii(i, m = md5_ii(m, f, r, i, d[n + 4], 6, -145523070), f, r, d[n + 11], 10, -1120210379), m, f, d[n + 2], 15, 718787259), i, m, d[n + 9], 21, -343485551), m = safe_add(m, h), f = safe_add(f, t), r = safe_add(r, g), i = safe_add(i, e) } return Array(m, f, r, i) } function md5_cmn(d, _, m, f, r, i) { return safe_add(bit_rol(safe_add(safe_add(_, d), safe_add(f, i)), r), m) } function md5_ff(d, _, m, f, r, i, n) { return md5_cmn(_ & m | ~_ & f, d, _, r, i, n) } function md5_gg(d, _, m, f, r, i, n) { return md5_cmn(_ & f | m & ~f, d, _, r, i, n) } function md5_hh(d, _, m, f, r, i, n) { return md5_cmn(_ ^ m ^ f, d, _, r, i, n) } function md5_ii(d, _, m, f, r, i, n) { return md5_cmn(m ^ (_ | ~f), d, _, r, i, n) } function safe_add(d, _) { var m = (65535 & d) + (65535 & _); return (d >> 16) + (_ >> 16) + (m >> 16) << 16 | 65535 & m } function bit_rol(d, _) { return d << _ | d >>> 32 - _ }

const deviceId = haDeviceFilePath.slice(haDeviceFilePath.lastIndexOf('/') + 1, -'.ha-device.yaml'.length)
const deviceHash = MD5(deviceId)

const yaml = require('js-yaml')

/**
 * @typedef {{
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
 *  features?: YamlFeature[]
 * }} YamlConfig

/**
 * @typedef {{
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
 *      flash_time_long?: number
 *      flash_time_short?: number
 *      resolution?: number
 *      pin?: string
 *      pinR?: string
 *      pinG?: string
 *      pinB?: string
 *      pinW?: string
 *      pinC?: string
 *      initial_state?: boolean
 *      initial_brightness?: number
 *      initial_temp?: number
 *      min?: number
 *      max?: number
 *      step?: number
 *      options?: string[]
 *      type?: 'button_short_press' | 'button_short_release' | 'button_long_press' | 'button_long_release' | 'button_double_press' | 'button_triple_press' | 'button_quadruple_press' | 'button_quintuple_press' | string
 *      subtype?: 'turn_on' | 'turn_off' | 'button_1' | 'button_2' | 'button_3' | 'button_4' | 'button_5' | 'button_6' | string
 *      mode?: "onoff" | "brightness" | "color_temp" | "hs" | "xy" | "rgb" | "rgbw" | "rgbww"
 * }} YamlFeature
 */

/**
 * @typedef {{
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
 *      supported_color_modes?: ("onoff" | "brightness" | "color_temp" | "hs" | "xy" | "rgb" | "rgbw" | "rgbww")[]
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
 * }} JsonFeature
 */

/**
 * @type {YamlConfig}
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

const startIdentifier = '// start\r\n'

/**
 * @type {[key: string]: string}
 */
const components = {}

for (const fileName of fs.readdirSync(`${__dirname}/components/`)) {
    const temp = fs.readFileSync(`${__dirname}/components/${fileName}`).toString()
    components[fileName.slice(0, -2)] = temp.slice(temp.indexOf(startIdentifier) + startIdentifier.length)
}

const discoveryPrefix = 'homeassistant'

/**
 * @type {JsonFeature[]}
 */
const haMqttJsonFeatures = []

let outputHeader = ''

if (device.time === true) { // default: false
    outputHeader += '#define TIME\n'
}
if (device.ota_update !== false) { // default: true
    outputHeader += '#define OTA_UPDATE\n'
}

/**
 * @type {string | null}
 */
const availabilityTopic = (device.availability !== false) ? `home/${toCodeName(device.name)}/availability` : null

outputHeader += components['manager']
    // uncomment all "// uncomment:..." commands,
    .replace(/\/\/ uncomment:/g, '')
    // replace DEVICE_ID_MD5_HASHED_HEXADECIMAL
    .replace(/DEVICE_ID_MD5_HASHED_HEXADECIMAL/g, deviceHash)
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
 * @type {string[]}
 */
const allFeaturesVarNames = []

/**
 *
 * @param {YamlFeature} feature
 * @param {JsonFeature} jsonFeature
 *
 * @returns {string | null}
 */
function processFeature(feature, jsonFeature) {
    /**
     * @type {string[]}
     */
    const componentLines = components[feature.class].split('\n')

    /**
     * @type {{condition: string, result: boolean}[]}
     */
    const conditionResultList = []

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
            case 'rgb_supported':
                conditionResult = feature.mode === 'rgb'
                break
            case 'hs_supported':
                conditionResult = feature.mode === 'hs'
                break
            case 'xy_supported':
                conditionResult = feature.mode === 'xy'
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

    /**
     * @type {Record<string,string>}
     */
    const defs = {}

    const pins = ['pin', 'pinR', 'pinG', 'pinB', 'pinW', 'pinC']

    const initials = ['intitial_state', 'intitial_brightness', 'intitial_temp']

    if (pins.some(pin => pin in feature)) {
        defs.OUTPUT_HANDLE = ''

        if (pins.some(pin => feature[pin]?.includes('\n')))
            throw new Error('feature config has multiline pin definition, which couldn\'t possibly result in a valid pin')

        for (const pin of pins) {
            if (!(pin in feature))
                continue

            let defName = 'OUTPUT_PIN'
            if (pin !== 'pin')
                defName += '_' + pin.at(-1)
            defs[defName] = feature[pin]
        }

        const maxBrightness = Math.pow(2, feature.resolution ?? 8) - 1

        if (feature.mode === 'onoff') {
            if (initials.some(init => init !== 'initial_state' && init in feature))
                throw new Error('feature config has an invalid \'initial_*\' set for "onoff" mode, only "initial_state" is allowed')

            if (pins.some(pin => pin !== 'pin' && pin in feature))
                throw new Error('feature config has multiple pins set, yet only "pin" is allowed for "onoff" mode')

            defs.OUTPUT_MODE_ONOFF = ''
            defs.INITIAL_BRIGHTNESS = (feature.initial_state ?? false) ? maxBrightness : 0
        }
        else if (feature.mode === 'brightness') {
            if (initials.some(init => init !== 'initial_brightness' && init in feature))
                throw new Error('feature config has an invalid \'initial_*\' set for "brightness" mode, only "initial_brightness" is allowed')

            defs.INITIAL_BRIGHTNESS = Math.floor((feature.initial_brightness ?? 0) / 100 * maxBrightness)
        }
        else if (feature.mode === 'color_temp') {
            if (initials.some(init => init !== 'initial_brightness' && init !== 'initial_temp' && init in feature))
                throw new Error('feature config has an invalid \'initial_*\' set for "color_temp" mode, only "initial_brightness" and "initial_temp" are allowed')

            const min = feature.min_kelvin ?? 2700
            const max = feature.max_kelvin ?? 6500
            const cur = feature.initial_temp ?? 4000

            if (cur < min)
                throw new Error(`feature config has initial_temp set to ${cur} when minimum defined is ${min}`)
            if (cur > max)
                throw new Error(`feature config has initial_temp set to ${cur} when maximum defined is ${max}`)

            const ratio = (cur - min) / (max - min)

            defs.INITIAL_BRIGHTNESS_W = Math.floor((feature.initial_brightness ?? 0) / 100 * maxBrightness * (1 - ratio))
            defs.INITIAL_BRIGHTNESS_C = Math.floor((feature.initial_brightness ?? 0) / 100 * maxBrightness * ratio)
        }

        // TODO: for smart RGB/RGBW/RGBWW
        // defs.OUTPUT_DATA = ''


        if (Object.keys(defs).length) {
            componentLines.unshift(...Object.entries(defs).map(([key, value]) => `#define ${key} ${value}`))
            componentLines.push(...Object.keys(defs).map((key) => `#undef ${key}`))
        }
    }
    else if (initials.some(init => init in feature)) {
        throw new Error('feature config has an \'initial_*\' property set, which requires a \'pin*\' property')
    }

    const varName = feature.var_name ?? toCodeName(jsonFeature.name).replace(/-/g, '_')
    allFeaturesVarNames.push(varName)

    // replace special keywords with content
    return componentLines.join('\n')
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
        .replace(/VAR_NAME/g, varName)
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

            if ('resolution' in feature && !(haMqtt.brightness && (!feature.mode || feature.mode === 'brightness' || feature.mode === 'color_temp')))
                throw new Error('resolution property is only allowed in brightness and color_temp modes, all other modes use 8-bit values');
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

outputHeader += `void _ha_device_init_features()\n{${allFeaturesVarNames.map(name => `\n    ${name}._init();`).join('')}\n}`

// insert discovery publish
outputHeader = outputHeader.replace('// __insert-discovery-publish\r\n', haMqttJsonFeatures.map(jsonFeature => `publish("${jsonFeature['~']}/config", R"=-=-=(${JSON.stringify(jsonFeature)})=-=-=", true);`).join('\n\t\t') + '\n')

outputHeader = outputHeader.replace(/^\s+$/gm, '\n')

// write the output file
fs.writeFileSync('include/ha-device.h', outputHeader)

// include ha-device and init it in main.cpp, if not already done
if (fs.existsSync('src/main.cpp')) {
    let mainFile = fs.readFileSync('src/main.cpp').toString()
    let changed = false

    if (!mainFile.includes('#include "ha-device.h"')) {
        let newline = '\n'
        if (!mainFile.includes('#include '))
            newline += '\n'
        mainFile = '#include "ha-device.h"' + newline + mainFile
        changed = true
    }

    if (mainFile.includes('#include <Arduino.h>\n')) {
        mainFile = mainFile.replace('#include <Arduino.h>\n', '')
        changed = true
    }

    if (!mainFile.includes('device.init(')) {
        const startSetup = mainFile.indexOf('\n{', mainFile.indexOf('void setup()'))
        const endSetup = mainFile.indexOf('\n}', startSetup)
        mainFile = mainFile.slice(0, endSetup) + '\n    device.init();' + mainFile.slice(endSetup)
        changed = true
    }

    if (!mainFile.includes('device.loop();')) {
        const startLoop = mainFile.indexOf('\n{', mainFile.indexOf('void loop()')) + 2
        mainFile = mainFile.slice(0, startLoop) + '\n    device.loop();' + mainFile.slice(startLoop)
        changed = true
    }

    if (changed)
        fs.writeFileSync('src/main.cpp', mainFile)
}

const libDeps = [
    'knolleary/PubSubClient@^2.8',
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
            iniFile += `upload_port = ${host}\nupload_flags = --host_port=9938\n`

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