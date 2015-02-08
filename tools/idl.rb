#!/usr/bin/env ruby

require 'rubygems'
require 'backports'
require 'json'
require 'optparse'
require 'ostruct'
require 'fileutils'
require 'open-uri'
require 'pathname'
require 'digest'
require 'erb'

class Renderer < OpenStruct

  def put_value
    return "hello"
  end

  def render(file)
    ERB.new( IO.read(file), nil, '-').result(binding)
  end
end

def type_from_value(value)
  if value.nil?
    return 'null'
  elsif value.is_a? TrueClass or value.is_a? FalseClass
    return 'bool'
  elsif value.is_a? Integer
    return 'int'
  elsif value.is_a? Float
    return 'double'
  elsif value.is_a? String
    return 'string'
  else
  end
end


def parse(file, template)

  data = JSON.parse( IO.read(file), :symbolize_names => true )

  data.each do |model, decl|

    properties = []
    decl.each do |name, value|
      if value.is_a? Hash
        value[:name] ||= name
        if value[:value]
          value[:type] ||= type_from_value(value[:value])
        else
          value[:type] = 'struct'
        end
        if value[:optional]
            value[:type] = "optional<#{value[:type]}>"
        end
        properties << value
      else
        properties << { :type => type_from_value(value), :name => name, :value => value }
      end
    end

    if template
      renderer = Renderer.new :name => model,
        :properties => properties.clone
      puts 'render'
      puts renderer.render( template ).to_s
    else
      render_cpp(model, properties.clone, commands)
      render_objc(model, properties.clone, commands)
      render_qt(model, properties.clone, commands)
    end
  end

end

def render_cpp(name, properties, commands)


  properties.each do |property|
      case property[:type]
      when 'null:'
        property[:cpptype] = 'std::nullptr_t'
      when 'bool'
        property[:cpptype] = 'bool'
      when 'int'
        property[:cpptype] = 'int'
      when 'double'
        property[:cpptype] = 'double'
      when 'string'
        property[:cpptype] = 'std::string'
      when /optional<(.*)>/
        inner_value = Regexp.last_match(1)
        if inner_value == 'string'
          inner_value = 'std::string'
        end
        property[:cpptype] = 'optional<' + inner_value + '>'
      end
  end

  renderer = Renderer.new :qtname => "Qt#{name}",
      :name => name,
      :properties => properties,
      :commands => commands

  IO.write("#{name.to_s.downcase}.hpp", renderer.render('cpp.hpp.erb') )
  IO.write("#{name.to_s.downcase}.cpp", renderer.render('cpp.cpp.erb') )

end

def render_qt(name, properties, commands)

  properties.each do |property|
      case property[:type]
      when 'bool'
        property[:qttype] = 'bool'
      when 'int'
        property[:qttype] = 'int'
      when 'double'
        property[:qttype] = 'double'
      when 'string'
        property[:qttype] = 'QString'
      when /optional/
        property[:qttype] = 'QVariant'
      end
    end

    renderer = Renderer.new :name => name,
      :properties => properties,
      :commands => commands,
      :qtname => "Qt#{name}"

    IO.write("Qt#{name}.h", renderer.render('qt.cpp.erb') )

end

def render_objc(name, properties, commands)

  properties.each do |property|
      case property[:type]
      when 'bool'
        property[:nstype] = 'BOOL'
      when 'int'
        property[:nstype] = 'int'
      when 'double'
        property[:nstype] = 'double'
      when 'string'
        property[:nstype] = 'NSString*'
      when /optional<(bool|int|double)>/
        property[:nstype] = 'NSNumber*'
      when /optional<string>/
        property[:nstype] = 'NSString*'
      end
  end


  renderer = Renderer.new :name => name,
      :properties => properties,
      :commands => commands
  IO.write( "#{name}.h", renderer.render('objc.h.erb'))
  IO.write( "#{name}.mm", renderer.render('objc.mm.erb'))
end

OptionParser.new do |opts|
  opts.banner = "Usage: #{$0}"
  opts.on("-v", "--[no-]verbose") do |value|
  end
  opts.on("-t", "--template=NAME") do |value|
    puts "tempalte = #{value}"
    $template = value
  end
end.parse!


ARGV.each do |file|
  parse(file, $template)
end
