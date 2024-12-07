#!/usr/bin/env node

// this will stub host, based on your api yaml files
import { readFile } from 'fs/promises'
import { basename } from 'path'
import YAML from 'yaml'

const [,progname, yamlfile] = process.argv
if (!yamlfile) {
  console.error(`Usage: ${basename(progname)} <YAML_FILE>`)
  process.exit(1)
}
const out = []

// map param/return types to C types
function typeMap(t) {
  if (['Image', 'Font', 'Sound', 'ImageFilter'].includes(t)) {
    return 'u32'
  }
  if (t === 'void') return 'void'
  if (t === 'string' || t.endsWith('[]') || t[0].toUpperCase() === t[0]) return 'u32'
  return t  // use the type directly from yaml (u8, u32, etc)
}

function generateBody(fname, func) {
  const lines = []
  const params = []

  // Handle input parameters
  for (const [name, type] of Object.entries(func.args)) {
    if (['Image', 'Font', 'Sound', 'ImageFilter'].includes(type)) {
      params.push(name)
    } else if (type === 'string') {
      lines.push(`  char* ${name} = copy_from_cart_string(${name}Ptr);`)
      params.push(name)
    } else if (type.endsWith('[]')) {
      lines.push(`  ${type.replace('[]', '')}* ${name} = copy_from_cart(${name}Ptr, ${name}Len);`)
      params.push(name, `${name}Len`)
    } else if (type[0].toUpperCase() === type[0]) {
      lines.push(`  ${type}* ${name} = copy_from_cart(${name}Ptr, sizeof(${type}));`)
      params.push(name)
    } else if (type.endsWith('*')) {
      params.push(name + 'Ptr')
    } else {
      params.push(name)
    }
  }

  // Filter out duplicate length parameters from params array
  const uniqueParams = [...new Set(params)]

  // Handle return value setup
  if (func.returns === 'string') {
    lines.push(`  // implement ${fname}(${uniqueParams.join(', ')}) here`)
    lines.push(`  char* s = "";`)
    lines.push(`  return copy_to_cart_string(s);`)
  } else if (func.returns.endsWith('[]')) {
    lines.push(`  u32 outLen = 0;`)
    lines.push(`  // implement ${func.returns.replace('[]', '')}* returnVal=${fname}(&outLen) here`)
    lines.push(`  ${func.returns.replace('[]', '')}* returnVal = {};`)
    lines.push(`  copy_to_cart_with_pointer(outLenPtr, &outLen, sizeof(outLen));`)
    lines.push(`  return copy_to_cart(returnVal, outLen);`)
  } else if (func.returns[0].toUpperCase() === func.returns[0]) {
    lines.push(`  // implement ${fname}(${params.join(', ')}) here`)
    lines.push(`  ${func.returns} result = {};`)
    lines.push(`  return copy_to_cart(&result, sizeof(result));`)
  } else if (func.returns === 'void') {
    lines.push(`  // implement ${fname}(${uniqueParams.join(', ')}) here`)
  } else {
    lines.push(`  // implement ${fname}(${uniqueParams.join(', ')}) here`)
    lines.push(`  return 0;`)
  }

  return lines.join('\n')
}

// Output function definitions
const api = YAML.parse(await readFile(yamlfile, 'utf8'))
const apiName = yamlfile.split('/').pop().split('.')[0]
out.push(`// ${apiName.toUpperCase()} API\n`)

for (const [fname, func] of Object.entries(api)) {
  out.push(`// ${func.description}`)

  // Convert arguments to include Ptr for pointer types
  const args = Object.entries(func.args).map(([name, type]) => {
    if (type === 'string') {
      return `${typeMap(type)} ${name}Ptr`
    }
    if (type.endsWith('[]')) {
      // Don't add length parameter here if it's already in the arguments
      const hasLengthParam = Object.entries(func.args).some(([n, t]) => n === `${name}Len`)
      const parts = [`${typeMap(type)} ${name}Ptr`]
      if (!hasLengthParam) parts.push(`${typeMap('u32')} ${name}Len`)
      return parts.join(', ')
    }
    if (type[0].toUpperCase() === type[0]) {
      return `${typeMap(type)} ${name}Ptr`
    }
    return `${typeMap(type)} ${name}${type.endsWith('*') ? 'Ptr' : ''}`
  }).join(', ')

  const returnType = typeMap(func.returns)
  out.push(`HOST_FUNCTION(${returnType}, ${fname}, (${args}), {`)
  out.push(generateBody(fname, func))
  out.push('})\n')
}


console.log(out.join('\n'))
