function PromptEditor(param) {
  var _this = this;

  // create editform dialog
  var date = new Date();
  var editor_id = 'oerp-editor-' + param.send.model.replace('.', '-') + '-'
      + date.getTime() + date.getMilliseconds();
  delete date;

  this.dialog = $(
      '<div id="' + editor_id + '" class="oerp-formeditor">Loading...</div>')
      .appendTo('body');

  this.dialog.data('param', param);
  this.dialog.data('v', param.v);
  this.dialog.data('prompt', this);
  this.dialog.data('field', param.field);
  this.dialog.data('getSend', this.getSend);

  // init dialog
  var option = {
    close : function(event, ui) {
      $(this).remove();
    },
    width : 974,
    modal : true
  };

  if (param.option) {
    option = $.extend(option, param.option);
  }

  this.dialog.dialog(option);
  this.setDialog(param);
}

PromptEditor.prototype.setDialog = function(param) {
  var _this = this;
  var send = param.send;

  _this.dialog.dialog('disable');
  $.post('?q=oerp/view/editor/js', send, function(data){
    _this.initDialog(data, param)
    _this.dialog.dialog('enable');
  });
};

PromptEditor.prototype.initDialog = function(data, param) {
  var _this = this;
  _this.dialog.html(decodeURIComponent(eval(data)));

  if (param.vals != undefined) {
    _this.dialog.find('*[retrieve]').each(function(){
      var ele = $(this);
      var src = ele.attr('retrieve');
      var funcLambda = new Function('dialog', 'ele', 'values', src);

      funcLambda(_this.dialog, ele, param.vals['#value']);
    });

    // update regular form eles
    _this.dialog
        .find(':input')
        .each(
        function() {
          $this = $(this);

          if ($this.parents('div.field:eq(0)').attr('oerp_type') == 'many2one') {
            var pat = /(.*)\[name\]/;
            var name = pat.exec($this.attr('name'));
            var val = param.vals['#value'][name[1]];
          } else {
            var val = param.vals['#value'][$this.attr('name')];
          }

          if (val == undefined
              || val['#value'] == undefined
              || $this.attr('no-override')
              || $this.attr('reload')){
            return;
          }

          switch ($this[0].type) {
            case 'hidden':
              $this.val(val['#value']);
              break;

            case 'textarea':
              $this.attr('value', val['#value']);
              break;

            case 'text':
              if (val['#text']) {
                // many2one
                $this.val(val['#text']).attr('rid', val['#value']);
              } else {
                $this.val(val['#value']);
              }
              break;

            case 'checkbox':
              if (val) {
                $this.attr('checked', 'checked');
              } else {
                $this.removeAttr('checked');
              }
              break;

            case 'select-one':
              $this.children('option[value="' + val['#value'] + '"]')
                  .attr('selected', 'selected');

              break;
          }
        });

    // update tree records
    _this.dialog.find('div.oerp-treeview').each(function() {
      var name = 'view[' + $(this).attr('oerp_name') + ']';
      var changes = param.vals['#value'][name];
      var v = new TreeView(this);

      for (var rid in changes) {
        if (rid.substr(0, 2) == '0-') {
          v.addTr(changes[rid], rid);
        } else if (changes[rid]['#value']) {
          v.updateTr(rid, changes[rid]);
        }

        if (changes[rid]['#delete']) {
          v.delTr(rid);
        }
      }
    });
  }

  _this.dialog.find('form.oerp-formview').oerpformview();
  _this.dialog.find('table.oerp-treeview').oerptreeview();

  // auto-adjust vertical position
  var wrapper = _this.dialog.parents('div.ui-dialog:eq(0)');
  var bottom = wrapper.height() + wrapper.offset().top + 10;
  var win_height = $(window).height();

  _this.dialog.dialog('option', 'position', 'center');
  height = wrapper.height() + wrapper.offset().top + 10;

  if ($('div.ui-widget-overlay').height() < height) {
    $('div.ui-widget-overlay').height(height);
  }
};

PromptEditor.prototype.getSend = function() {
  var $form = $(this).find('form:eq(0)');
  var send = {
    '#value' : {}
  };

  // sync treeview records
  $form.find('div.oerp-treeview').each(
      function() {
        var $this = $(this);
        var send_inner = {};

        $this.find('tr[oerp_rid]').each(function() {
          var $this = $(this);

          if ($this.data('val')) {
            send_inner[$this.attr('oerp_rid')] = $this.data('val');
          }
        });

        var fld_name = $this.children('input[type="hidden"]:eq(0)').attr(
            'name').replace('[json]', '');

        send['#value'][fld_name] = send_inner;
      }
  );

  var post = $form.serializeArray();
  $form.find('input:checkbox').each(function() {
    var $ele = $(this);

    if ($ele.attr('checked') == false) {
      post.push({
        name : $(this).attr('name'),
        value : false
      });
    }
  });

  var required_pass = true;
  
  // prepare send{}
  for (var fld in post) {
    var $ele = $form.find('[name="' + post[fld]['name'] + '"]');
    var ele_type = $ele.parents('div[oerp_type]:eq(0)').attr('oerp_type');

    if($ele.hasClass('required') && !post[fld]['value']){
      $ele.addClass('error');
      required_pass = false;
    }

    var src = $ele.attr('prepareSend');
    if(!$ele.attr('no-override') && src){
      var funcLambda = new Function('form', 'ele', 'fld', src);
      var resp = funcLambda($form, $ele, post[fld]);
      send['#value'][resp['#name']] = resp;
    }
  }

  return !required_pass ? false : send;
};