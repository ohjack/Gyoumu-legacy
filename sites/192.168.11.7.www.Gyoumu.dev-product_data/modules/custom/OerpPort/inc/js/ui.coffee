class OerpJsEditor extends window.UI.Abstract
  getSend: =>
    $form = $(@.$node).find('form:eq(0)')
    send = '#value' : {}

    # sync treeview records
    $form.find('div.oerp-treeview').each ->
      $treeview = $(this);
      send_inner = {};

      $treeview.find('tr[oerp_rid]').each ->
        $tr = $(this)

        if $tr.data('vals')
          send_inner[$tr.attr('oerp_rid')] = $tr.data('vals')

      fld_name = $treeview.children('input[type="hidden"]:eq(0)').attr('name').replace('[json]', '')
      send['#value'][fld_name] = send_inner

    post = $form.serializeArray()

    $form.find('input:checkbox').each ->
      $checkbox = $(this)

      if $checkbox.attr('checked') == false
        post.push(
          name : $checkbox.attr('name')
          value : false
        )

    required_pass = true

    # prepare send{}
    for val in post
      $ele = $form.find("[name='#{val['name']}']")
      ele_type = $ele.parents('div[oerp_type]:eq(0)').attr('oerp_type')

      if $ele.hasClass('required') && !val['value']
        $ele.addClass('error')
        required_pass = false

      src = $ele.attr('prepareSend')
      if !$ele.attr('no-override') and src
        funcLambda = new Function('form', 'ele', 'fld', src)
        resp = funcLambda($form, $ele, val)
        send['#value'][resp['#name']] = resp

    return if !required_pass then false else send

  save: (func_aftersave)=>
    view = @.$node.find('div.oerp-view:eq(0)')
    model = view.attr('oerp_model')
    rid = Number(view.attr('oerp_rid'))

    if rid == 0
      send = [model, 'create', @.getSend()]
    else
      send = [model, 'write', [ rid ], @.getSend()]

    $.post(
      '?q=oerp/execute/js',
      { send: JSON.stringify(send) },
      (rs)->
        func_aftersave(rs)
    )

  reload: (send, extend = {})=>
    if send?
      final_send = send
    else
      final_send = @.clone(@.$node.data('send_bak'))

    final_send = $.extend(final_send, extend)
    @.$node.data('send', final_send)
    @.init()

  reset: =>
    send_bak = @.clone(@.$node.data('send_bak'))
    @.$node.data('send', send_bak)
    @.init()

  setDVals: =>
    dvals = @.data('dvals')

    if dvals['#value']?
      @.$node.find('*[retrieve]').each (ind, ele)=>
        $ele = $(ele)
        src = $ele.attr('retrieve')

        funcLambda = new Function('dialog', 'ele', 'values', src)
        funcLambda(@.$node, $ele, dvals['#value'])

      # update regular form eles
      @.$node
        .find(':input')
        .each (ind, ele)=>
          $ele = $(ele)

          if $ele.parents('div.field:eq(0)').attr('oerp_type') == 'many2one'
            pat = /(.*)\[name\]/
            name = pat.exec($ele.attr('name'))
            val = dvals['#value'][name[1]]

          else
            val = dvals['#value'][$ele.attr('name')]

          if not val? or
             not val['#value']? or
             $ele.attr('no-override') or
             $ele.attr('reload')

            return null

          switch $ele[0].type
            when 'hidden'
              $ele.val(val['#value'])

            when 'textarea'
              $ele.attr('value', val['#value'])

            when 'text'
              if val['#text']
                # many2one
                $ele.val(val['#text']).attr('rid', val['#value'])

              else
                $ele.val(val['#value'])

            when 'checkbox'
              if val
                $ele.attr('checked', 'checked')
              else
                $ele.removeAttr('checked')

            when 'select-one'
              $ele.children('option[value="' + val['#value'] + '"]')
                .attr('selected', 'selected')

      # update tree records
      @.$node.find('div.oerp-treeview').each (ind, ele)=>
        $ele = $(ele)
        name = "view[#{$(ele).attr('oerp_name')}]"
        changes = dvals['#value'][name]

        if changes?
          UI = $ele.parents("[data-awake='OerpTreeWidget']:eq(0)").data('UI')

          for rid, change of changes
            if rid.substr(0, 2) == '0-'
              UI.addTr(change, rid)

            else if change['#value']
              UI.updateTr(change, rid)

            if (change['#delete'])
              UI.delTr(rid)

  init: =>
    @.$node.html('<em>Loading...</em>')

    $.post(
      '?q=oerp/view/editor/js',
      @.data('send'),
      (json)=>
        @.$node.html(decodeURIComponent(eval(json)))
        new window.UI.Awaker(@.$node)
        @.setDVals()
        @.$node.oerpformview()
        @.data('func_afterawake', true)()
    )

  createEle: (send, dvals)=>
    ele = $('<div class="awakable" data-awake="OerpJsEditor"/>')

    ele.data('send', send)
    ele.data('send_bak', @.clone(send))
    ele.data('dvals', dvals)
    ele

window.UI.OerpJsEditor = OerpJsEditor


class OerpPrompt extends window.UI.Abstract
  getName: => "OerpPrompt"

  close: =>
    @.$node.dialog('destroy').remove()

  getOption: =>
    close: => @.close()
    width: 974
    modal: true
    autoOpen: false

  adjustPosition: =>
    @.$node.dialog('option', 'position', 'center')

    wrapper = @.$node.parents('div.ui-dialog:eq(0)')
    height = wrapper.height() + wrapper.offset().top + 10;
    overlay = $('div.ui-widget-overlay')

    if overlay.height() < height
      overlay.height(height)

  init: =>
    $inner = @.$node.data('$inner')
    $inner.data('UIParent', @)

    @.$node.dialog(@.getOption())
    @.$node.html($inner)

    new window.UI.Awaker($inner, => @.adjustPosition())
    @.$node.dialog('open')

  createEle: (UIParent, $inner)=>
    pool = $('div#dialog-pool');

    if !pool.length
      pool = $('<div id="dialog-pool"></div>').appendTo('body')

    src = "<div class='awakable' data-awake='#{@.getName()}'/>"
    ele = $(src).appendTo(pool);

    ele.data('UIParent', UIParent).data('$inner', $inner)
    ele

window.UI.OerpPrompt = OerpPrompt


class OerpJsEditorPrompt extends window.UI.OerpPrompt
  getName: => "OerpJsEditorPrompt"

  createEle: (UIParent, send, dvals = null)=>
    $inner = new window.UI.OerpJsEditor().createEle(send)
    $inner.data('dvals', dvals)
    super(UIParent, $inner)

window.UI.OerpJsEditorPrompt = OerpJsEditorPrompt


class OerpPromptForm extends window.UI.OerpJsEditorPrompt
  getName: => "OerpPromptForm"

  save: =>
    Inner = @.$node.data('$inner').data('UI')

    Inner.save (rs)=>
      rid = eval(rs)
      UIParent = @.$node.data('UIParent')
      UIParent.changeRId(rid)
      @.close()

  getOption: =>
    opts = super()

    opts['buttons'] =
      Save: => @.save()

    opts

window.UI.OerpPromptForm = OerpPromptForm


class OerpRowPromptEditor extends window.UI.OerpJsEditorPrompt
  getName: => "OerpRowPromptEditor"

  update: =>
    send = @.$node.data('$inner').data('UI').getSend()

    if send
      @.$node.data('UIParent').modifyTr(send);
      @.$node.remove()

    else
      alert('Missing values in required fields. Please check again.')

  getOption: =>
    opts = super()

    opts['buttons'] =
      Update: => @.update()

    opts

window.UI.OerpRowPromptEditor = OerpRowPromptEditor


class OerpTreeWidget extends window.UI.Abstract
  getName: => "OerpTreeWidget"

  getView: =>
    @.$node.find('div.oerp-view')

  getTree: =>
    tree = @.getView().find('table.oerp-treeview:eq(0)')
    return if tree.length > 0 then tree else false

  getModel: (level = 0)=>
    mod = @.$clicked
      .parents("[oerp_model]:eq(#{level})")
      .attr('oerp_model')

    return mod ? ''

  getVId: (level = 0) =>
    vid = @.$clicked.parents("[oerp_vid]:eq(#{level})").attr('oerp_vid')
    if vid? then vid else ''

  getEleName: =>
    @.getView().attr('oerp_name')

  getRId: =>
    @.getTr().attr('oerp_rid')

  getCacheId: =>
    @.getView().attr('cache_id')

  getTpl: =>
    tr = @.getTree().find('tr.oerp-tpl')
    if tr.length > 0 then tr else false

  addTpl: (vals, rid)=>
    if rid?
      rid = "0-" + @.getTree().find('tr.oerp-new').length

    newTr = @.getTpl().clone(true)
    newTr
      .attr('oerp_rid', rid)
      .removeClass('oerp-tpl')
      .removeClass('hide')
      .addClass('oerp-new');

    @.getTree().removeClass('empty').find('tbody:eq(0)').append(newTr)
    @.updateTr(vals, rid)

  updateTr: (vals, rid) =>
    $tr = @.$node.find("tr[oerp_rid='#{rid}']");
    $tr.data('vals', vals);

    $tr.children('td[oerp_field]').each (ind, ele)=>
      $td = $(ele)
      val = vals['#value']["view[#{$td.attr('oerp_field')}]"]

      if val?
        @.updateTd(val, $td)

      else if $td.attr('gu_alt_name')
        val = vals['#value']["view[#{$td.attr('gu_alt_name')}]"]
        @.updateTd(val, $td)

      else if $td.children('span.oerp-changed').length == 0
        $td.html("<span class='oerp-changed'>#{$td.html()}</span>");

    $tr.addClass('toChange')
    $tr.nextAll('tr').each (ind, ele)=>
      next_tr = $(ele)

      if !next_tr.attr('merged')
        false

      else
        next_tr.addClass('toChange')

    @.initBtns($tr);

  addTr: (vals) =>
    tail = @.getTree().find('tr.oerp-new').length
    rid = "0-#{tail}"

    $newTr = @.getTpl().clone(true)
    $newTr
      .attr('oerp_rid', rid)
      .removeClass('oerp-tpl')
      .removeClass('hide')
      .addClass('oerp-new');

    @.getTree().removeClass('empty').find('tbody:eq(0)').append($newTr);
    @.updateTr(vals, rid);

  delTr: (rid = null)=>
    tr = @.getTr(rid)

    if tr?
      val = tr.data('vals')

      if tr.hasClass('toDelete')
        delete val['#delete'];
        if val['#value']? then tr.data('vals', val) else tr.removeData('vals')

      else if val?
        val['#delete'] = true;
        tr.data('val', val);

      else
        tr.data('vals', '#delete' : true)

      tr.toggleClass('toDelete')
      tr.nextAll('tr').each (ind, ele)=>
        next_tr = $(ele)

        if next_tr.attr('merged')
          next_tr.toggleClass('toDelete')

        else
          false

  modifyTr: (vals)=>
    if @.getRId() == "0"
      @.addTr(vals)

    else
      @.updateTr(vals, @.getRId())

  updateTd: (val, $td) =>
    if val['#text']
      $td.html(val['#text'])

    else
      $td.html(val['#value'])

  catchClk: (ev) =>
    @.$clicked = $(ev.target)
    @

  editTr: =>
    send =
      par_model : @.getModel(1),
      par_vid : @.getVId(1),
      model : @.getModel(0),
      vid : @.getVId(0),
      name : @.getEleName(),
      rid : @.getRId(),
      cache_id : @.getCacheId(),
      type : 'form',

    dvals = @.getTr().data('vals')
    $ele = new window.UI.OerpRowPromptEditor().createEle(@, send, dvals)
    new window.UI.Awaker($ele)

  getTr: (rid = null)=>
    if rid?
      return @.$node.find("tr[oerp_rid='#{rid}']:eq(0)")

    else if @.$clicked.get(0).tagName is 'TR'
      @.$clicked

    else
      @.$clicked.parents('tr:eq(0)')

  clearSel: =>
    view = @.getView()
    view.find('tr.selected').removeClass('selected')
    view.find('td.oerp-op div.oerp-button')
      .addClass('ui-state-default')
      .removeClass('ui-state-highlight');

  selTr: =>
    tr = @.getTr()
    op = tr.find('td.oerp-op div.oerp-button')

    if tr?
      tr.toggleClass('selected')
      tr = tr.next('tr')

      while tr.length
        if tr.attr('merged')
          tr.toggleClass('selected')
          tr = tr.next('tr');

        else
          break

  initBtns: =>
    @.$node.find('.oerp-btn-del').unbind('click').click (ev)=>
      @.catchClk(ev).delTr()

    @.$node.find('.oerp-btn-chk, .oerp-btn-select').unbind('click').click (ev)=>
      @.catchClk(ev).selTr()

    @.$node.find('.oerp-btn-edit').unbind('click').click (ev)=>
      if !@.catchClk(ev).getTr().hasClass('toDelete')
        @.editTr()

    @.$node.find('.oerp-btn-add').unbind('click').click (ev)=>
      @.catchClk(ev).getTr().parents('table:eq(0)').find('tr[oerp_rid="0"] .oerp-btn-edit').click()

    tree = @.getTree()
    tree.prev('table.sticky-header:eq(0)').find('.oerp-btn-add').click (ev)=>
      $(ele).parents('div.oerp-treeview:eq(0)')
        .find('table.oerp-treeview div.oerp-btn-add').click()

    if tree.hasClass('select-one')
      tree.find('tr[oerp_rid]').click (ev)=>
        @.catchClk(ev)
        @.clearSel()
        @.selTr()

    sig = tree.attr('signature')
    $('thead .oerp-btn-select').unbind('click').click (ev)=>
      $chk = $(ev.target)
      selected = $chk.hasClass('selected')
      $chk.toggleClass('selected')

      $trs = $('tbody div.oerp-btn-select[signature=' + sig + ']:eq(0)')
        .parents('table:eq(0)').find('tr').not('.oerp-tpl')

      if selected
        $trs.removeClass('selected')

      else
        $trs.filter('.sep-after').removeClass('selected')
        $trs.not('.sep-after').addClass('selected')

  init: =>
    @.initBtns()

window.UI.OerpTreeWidget = OerpTreeWidget


class OerpJsRecordSelector extends window.UI.OerpJsEditorPrompt
  getName: => "OerpJsRecordSelector"

  select: =>
    UIParent = @.$node.data('UIParent')
    $inner = @.$node.data('$inner')
    rid = $inner.find('tr.selected').attr('oerp_rid')

    if rid?
      UIParent.changeRId(rid)

    @.close()

  formview: =>
    UIParent = @.$node.data('UIParent')
    @.close()

    send = UIParent.getFormSendData()
    send.rid = 0
    UIParent.createPrompt(UIParent, send)

  getOption: =>
    opts = super()

    opts['maxHeight'] = 400
    opts['buttons'] =
      OK: => @.select()
      New: => @.formview()

    opts

window.UI.OerpJsRecordSelector = OerpJsRecordSelector


class OerpMany2oneWidget extends window.UI.Abstract
  getName: => "OerpMany2oneWidget"

  getInput: =>
    @.$node.find('input.form-text')

  initUpdate: =>
    @.getInput().change =>
      @.update()

  getModel: =>
    @.getInput().attr('model')

  getEleName: =>
    @.getInput().attr('name');

  getRid: =>
    Number(@.getInput().attr('rid'))

  getFormSendData: =>
    model: @.getModel()
    name: @.getEleName()
    rid: @.getRid()
    type: 'form'

  createPrompt: (UIParent, send)=>
    $ele = new window.UI.OerpPromptForm().createEle(
      UIParent, send)

    new window.UI.Awaker($ele)

  initEdit: =>
    @.$node.find('div.oerp-btn-edit').click (ev)=>
      $btn = $(ev.target)

      if $btn.hasClass 'ui-state-disabled'
        false

      @.createPrompt(@, @.getFormSendData())

  getDomain: =>
    @.getInput().attr('domain') ? null

  getSearchSendData: =>
    model: @.getModel()
    name: @.getName()
    type: 'tree'
    domain: @.getDomain()

  initSearch: =>
    @.$node.find('div.oerp-btn-search').click (ev)=>
      $btn = $(ev.target)
      if $btn.hasClass('ui-state-disabled')
        false

      $ele = new window.UI.OerpJsRecordSelector().createEle(
        @, @.getSearchSendData())

      new window.UI.Awaker($ele)

  update: =>
    send = [ @.getModel(), 'name_get', [ @.getRid() ] ]

    if @.$node.parents('div.searchpanel:eq(0)').length
      return false

    $.post(
      '?q=oerp/execute/js',
      send : JSON.stringify(send),
      (data) =>
        resp = eval(data);
        @.getInput().attr('value', resp[0][1])
    )

  changeRId: (rid) =>
    if typeof rid isnt 'boolean'
      @.getInput().attr('rid', rid)

    @.update()
    @.getInput().change()

  init : =>
    @.initUpdate()
    @.initEdit()
    @.initSearch()

window.UI.OerpMany2oneWidget = OerpMany2oneWidget


class OerpTabs extends window.UI.Abstract
  getName: => "OerpTab"

  init: =>
    @.$node.tabs();

window.UI.OerpTabs = OerpTabs


class OerpSearchPanel extends window.UI.Abstract
  getName: => "OerpSearchPanel"

  getJsEditor: =>
    @.$node.parents('[data-awake="OerpJsEditor"]:eq(0)').data('UI')

  getForm: =>
    @.$node.parents('form:eq(0)')

  isDialog: =>
    @.$node.parents('div.ui-dialog:eq(0)').length > 0

  buildDomain: ()=>
    $form = @.getForm()
    post = $form.serializeArray()

    pats =
      genernal: /.*\[_search\]\[([^\[]*)\]$/
      float: /.*\[_search\]\[([^\[]*)\]\[(GE|LE)\]$/

    domain = []

    for fld in post
      for pat_type, pat of pats
        m = pat.exec(fld.name)

        if !m || !m[1]
          continue

        if fld.value.length
          if (m[2])
            switch m[2]
              when 'GE'
                domain.push([ m[1], '>=', fld.value ])

              when 'LE'
                domain.push([ m[1], '<=', fld.value ])
          else
            $ele = $form.find("[name='#{fld.name}']")
            val = fld.value

            switch $ele.parents('[oerp_type]:eq(0)').attr('oerp_type')
              when 'boolean'
                switch val
                  when 'null'
                    continue

                  when 'yes'
                    val = 1

                  when 'no'
                    val = 0

            domain.push([ m[1], $ele.attr('op'), val ])

    domain

  clickFind: (ev)=>
    ev.preventDefault()
    UISelector = @.getJsEditor()
    send = @.clone(UISelector.$node.data('send'))
    send.domain = JSON.stringify(@.buildDomain())

    @.getJsEditor().reload(send)

  clickClear: (ev)=>
    ev.preventDefault()
    @.getJsEditor().reset()

  init: =>
    if !@.isDialog()
      return false

    $form = @.getForm()

    $form.find('input[name="func:find"]').click (ev)=>
      @.clickFind(ev)

    $form.find('input[name="func:clear"]').click (ev)=>
      @.clickClear(ev)

window.UI.OerpSearchPanel = OerpSearchPanel