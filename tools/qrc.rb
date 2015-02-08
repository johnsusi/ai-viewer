require 'erb'
require 'ostruct'

class Renderer < OpenStruct

  def render
    template = File.join(File.expand_path(File.dirname(__FILE__)), 'qrc.erb')
    ERB.new( IO.read(template), nil, '-').result(binding)
  end
end

ARGV.each do |pattern|

  renderer = Renderer.new :files => Dir.glob(pattern).select { |file| File.file?(file) }
  puts renderer.render

end
