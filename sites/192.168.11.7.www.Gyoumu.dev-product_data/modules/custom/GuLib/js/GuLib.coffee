$(document).ready ->
  new window.UI.Awaker $('body')

window.UI = []

class Awaker
  constructor: ($ele, func_afterawake) ->
    $ele.find('.awakable').andSelf().filter('.awakable').each ->
      $node = $(this)
      $node.removeClass('awakable')
      $node.data('func_afterawake', func_afterawake)
      awake = $node.attr 'data-awake'
      UIClass = window.UI[awake];

      if UIClass?
        new UIClass().createNew($node)

      else
        throw "Missing component '#{awake}'."

window.UI.Awaker = Awaker


class Abstract
  createNew: ($node) ->
    $node.data('UI', @)
    @.$node = $node
    @.init()
    @

  clone: (target) =>
    $.extend({}, target)

  data: (key, ref = false) =>
    data = @.$node.data(key)
    return if ref then data else @.clone(data)

window.UI.Abstract = Abstract