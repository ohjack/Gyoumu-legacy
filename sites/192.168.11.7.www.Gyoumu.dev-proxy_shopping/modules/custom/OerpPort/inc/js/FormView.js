(function($) {
  var form_methods = {
    init : function(params) {
      return this.each(function() {
        new FormView(this);
      });
    },
    update : function() {
      return this.each(function() {
        var v = new FormView(this);
        v.prepareSubmit();
      });
    }
  };

  $.fn.oerpformview = function(method) {
    if (form_methods[method]) {
      return form_methods[method].apply(this, Array.prototype.slice.call(
          arguments, 1));
    } else if (typeof method === 'object' || !method) {
      return form_methods.init.apply(this, arguments);
    }
  }
})(jQuery);

FormView.prototype = new FormEvent();

function FormView(ele) {
  var _this = this;

  _this.form = $(ele);
//  _this.initTabs();
  _this.initIconHover();
  _this.initTooltip();
  _this.initDatepicker();
  _this.initCkeditor();
//  _this.initTreeSearch();
  _this.initOnchange();
  _this.initButtonObject();
  _this.initButtonAction();
  _this.initButtonState();
  _this.initButtonWorkflow();
  _this.initReload();
}

FormView.prototype.getEditor = function()
{
  return this.form.parents('div.oerp-formeditor:eq(0)');
};

FormView.prototype.getPrompt = function()
{
  return this.getEditor().data('prompt');
};

FormView.prototype.getParam = function()
{
  return this.getEditor().data('param');
};

FormView.prototype.reloadDialog = function(param)
{
  this.getPrompt().setDialog(param);
};

FormView.prototype.isDialog = function() {
  return this.form.parents('div.ui-dialog:eq(0)').length
};

FormView.prototype.initTabs = function() {
  this.form.find('[id^="oerp-tabs"]').tabs();
};

FormView.prototype.initIconHover = function() {
  var _this = this;
  _this.form.find('.ui-icon').hover(
      function() {
        $(this).parent('div').addClass('ui-state-hover');
      },
      function() {
        $(this).parent('div').removeClass('ui-state-hover');
      }
  );
};

FormView.prototype.initTooltip = function() {
  var _this = this;
  _this.form.find('div.description').each(
      function() {
        $(this).prevAll('label:eq(0)').children('sup').hover(
            function(e) {
              var $this = $(this);
              var text = $this.parent().nextAll('div.description');
              var offset = $this.offset();

              text.clone()
                  .attr('id', 'tooltip')
                  .css('position', 'absolute')
                  .css('top', offset.top + $this.height() + 5)
                  .css('left', offset.left)
                  .appendTo('body')
                  .show();

              return false;

            },
            function(e) {
              $('#tooltip').remove();
            }
        );
      }
  );
};

FormView.prototype.initDatepicker = function() {
  var _this = this;

  _this.form.find('input.form-text[widget="datepicker"]').each(function() {
    var ele = $(this);

    if (!ele.attr('readonly')) {
      ele.attr('autocomplete', 'off');
      ele.datepicker({
        duration : 0,
        dateFormat : 'yy-mm-dd'
      });
    }
  });

  _this.form.find('input.form-text[widget="datetimepicker"]').each(function() {
    var ele = $(this);

    if (!ele.attr('readonly')) {
      ele.attr('autocomplete', 'off');
      ele.datetimepicker({
        duration : 0,
        showSecond : true,
        dateFormat : 'yy-mm-dd',
        timeFormat : 'hh:mm:ss',
        stepMinute : 15,
        stepSecond : 15
      });
    }
  });
};

FormView.prototype.initCkeditor = function() {
  var _this = this;
  var cke_config = {
    height: 400,
    toolbar:
        [
          ['Source'],
          ['Cut','Copy','Paste','PasteText','PasteFromWord','-','SpellChecker', 'Scayt'],
          ['Undo','Redo','Find','Replace','-','SelectAll','RemoveFormat'],
          ['Image','Flash','Table','HorizontalRule','Smiley','SpecialChar'],
          ['Maximize', 'ShowBlocks'],
          '/',
          ['Format'],
          ['Bold','Italic','Underline','Strike','-','Subscript','Superscript'],
          ['NumberedList','BulletedList','-','Outdent','Indent','Blockquote'],
          ['JustifyLeft','JustifyCenter','JustifyRight','JustifyBlock','-','BidiRtl','BidiLtr'],
          ['Link','Unlink','Anchor','Linkit','LinkToNode','LinkToMenu'],
          ['DrupalBreak', 'DrupalPageBreak']
        ]
  };

  _this.form.find('textarea.enable-ckeditor').each(function(){
    var ele = $(this);
    var instance = CKEDITOR.instances[ele.attr('id')];
    if(instance) {
      CKEDITOR.remove(instance);
    }
    ele.ckeditor(cke_config);
  });
};

FormView.prototype.initTreeSearch = function() {
  var _this = this;

    // init tree search
    if (_this.isDialog()) {
      var param = _this.getParam();

      _this.form.find('input[name="func:find"]').click(function(ev) {
        ev.preventDefault();
        post = _this.form.serializeArray();

        var send = [];
        for (var c = 0; c < post.length; c++) {
          var pat = {
            genernal : /.*\[_search\]\[([^\[]*)\]$/,
            float : /.*\[_search\]\[([^\[]*)\]\[(GE|LE)\]$/
          };

          for (var i in pat) {
            var m = pat[i].exec(post[c].name);

            if (!m || !m[1]) {
              continue;
            }

            if (post[c].value.length) {
              if (m[2]) {
                switch (m[2]) {
                  case 'GE':
                    send.push([ m[1], '>=', post[c].value ]);
                    break;

                  case 'LE':
                    send.push([ m[1], '<=', post[c].value ]);
                    break;
                }
              } else {
                var ele = _this.form.find('[name="' + post[c].name + '"]');
                var val = post[c].value;

                switch (ele.parents('[oerp_type]:eq(0)').attr('oerp_type')) {
                  case 'boolean':
                    switch (val) {
                      case 'null':
                        continue;

                      case 'yes':
                        val = 1;
                        break;

                      case 'no':
                        val = 0;
                        break;
                    }
                    break;
                }
                send.push([ m[1], ele.attr('op'), val ]);
              }
            }
          }
        }
        param.send.domain = JSON.stringify(send);
        _this.reloadDialog(param);
      });

      _this.form.find('input[name="func:clear"]').click(function(ev) {
        ev.preventDefault();
        param.send.domain = null;
        _this.reloadDialog(param);
      });
    }
};
/*
UI.components.OerpMany2oneWidget = {
  createNew: function() {
    var obj = {};

    obj.getInput = function() {
      return obj.$node.find('input.form-text');
    };

    obj.getName = function() {
      return obj.getInput().attr('name');
    };

    obj.getDomain = function() {
      return obj.getInput().attr('domain');
    };

    obj.getModel = function() {
      return obj.getInput().attr('model');
    };

    obj.getRid = function() {
      return Number(obj.getInput().attr('rid'));
    };

    obj.update = function() {
      var send = [ obj.getModel(), 'name_get', [ obj.getRid() ] ];

      if (obj.$node.parents('div.searchpanel:eq(0)').length) {
        return;
      }

      $.post('?q=oerp/execute/js', {
            send : JSON.stringify(send)
          },
          function(data) {
            var resp = eval(data);
            obj.getInput().attr('value', resp[0][1]);
          }
      );
    };

    obj.changeRId = function(rid) {
      if (typeof rid !== 'boolean') {
        obj.getInput().attr('rid', rid);
      }
      obj.update();
    };

    obj.initUpdate = function() {
      obj.getInput().change(function () { obj.update(); });
    };

    obj.getFormSendData = function () {
      var _this = this;
      return  {
        model: _this.getModel(),
        name: _this.getName(),
        rid: _this.getRid(),
        type: 'form'
      };
    };

    obj.initEdit = function(){
      var _this = this;

      _this.$node.find('div.oerp-btn-edit').click(function () {
        var $btn = $(this);
        if($btn.hasClass('ui-state-disabled')){
          return false;
        }

        UI.components.OerpPromptForm.createEle(
            _this, _this.getFormSendData()).awake();
      });
    };

    obj.getSearchSendData = function () {
      var _this = this;
      return {
        model : _this.getModel(),
        name : _this.getName(),
        type : 'tree',
        domain : _this.getDomain()
      };
    };

    obj.initSearch = function() {
      var _this = this;
      _this.$node.find('div.oerp-btn-search').click(
          function() {
            var $btn = $(this);

            if($btn.hasClass('ui-state-disabled')){
              return false;
            }

            UI.components.OerpPromptMany2oneSelector.createEle(
                _this, _this.getSearchSendData()).awake();
          }
      );
    };

    obj.init = function() {
      obj.initUpdate();
      obj.initEdit();
      obj.initSearch();
    };
    return obj;
  }
};
*/
FormView.prototype.funcOnchange = function() {
  var $this = $(this);
  var view = $this.parents('div.oerp-view:eq(0)');
  var pat = /([^(]*)\((.*)\)/;
  var m = pat.exec($this.attr('on_change'));
  var send = [ view.attr('oerp_model'), m[1], [] ];
  var args = m[2].split(',');

  for (var c = 0; c < args.length; c++) {
    var name = jQuery.trim(args[c]);
    var matchParent = name.match(/^parent\.(.*)/);

    if (matchParent) {
      name = matchParent[1];
    }

    var fld_sel = 'div[oerp_name="' + name + '"]:eq(0)';

    if (matchParent) {
      var dia = view.parents('div.ui-dialog:eq(0)');
      var dia_parent = dia.prevAll('div.ui-dialog');

      if (dia_parent.length) {
        var fld = dia_parent.find(fld_sel);

      }
      else {
        var fld = $('body form.oerp-formview:eq(0)').find(fld_sel);
      }
    }
    else {
      var fld = view.find(fld_sel);
    }

    switch (fld.attr('oerp_type')) {
      case 'many2one':
        var val = fld.find('input[rid]:eq(0)');

        if (val.length && val.attr('rid')) {
          val = Number(val.attr('rid'));
        }
        else {
          val = false;
        }
        break;

      case 'int':
      case 'float':
        var val = Number(fld.find('input').val());
        break;

      default:
        var val = fld.val();

        if (!val) {
          val = false;
        }
        break;
    }
    send.push(val);
  }

  $.post(
      '?q=oerp/execute/js',
      {
        send : JSON.stringify(send),
        onchange : true
      },
      function(data) {
        // var resp = JSON.parse(data);
        var resp = eval('(' + data + ')');

        for (x in resp.value) {
          if (resp.value[x] === false) {
            continue;
          }

          var fld = view.find('div[oerp_name="' + x + '"]:eq(0)');

          if (fld.attr('invisible')) {
            fld.find('input[type="hidden"]').val(resp.value[x]);
            continue;
          }

          switch (fld.attr('oerp_type')) {
            case 'int':
            case 'float':
            case 'char':
            case 'date':
            case 'datetime':
              fld.find('input.form-text').val(resp.value[x]);
              break;

            case 'many2one':
              fld.find('input.form-text').attr('rid', resp.value[x]).change();
              break;
          }
        }
      }
  );
};

FormView.prototype.initOnchange = function() {
  var _this = this;
  _this.form.find('div[oerp_type] *[on_change]').change(_this.funcOnchange);
};

FormView.prototype.getSend = function()
{
  var domPrompt = this.getPrompt().dialog[0];
  return this.getEditor().data('getSend').call(domPrompt);
};

FormView.prototype.initReload = function() {
  var _this = this;
  _this.form.find('div[oerp_type] *[reload]').change(function(){
    var JsEditor = _this.form.data('UI');
    var vals = JSON.stringify(JsEditor.getSend());
    JsEditor.reload(null, {overrides: vals})
  });
};

FormView.prototype.initButtonObject = function() {
  var _this = this;

  _this.form.find('input[type="submit"][button_type="object"]').click(
      function(ev) {
        if (_this.isDialog()) {
          ev.preventDefault();

          var btn = $(this);
          btn.parents('.oerp-formeditor:eq(0)').dialog('disable');

          $.post(
              '?q=oerp/execute/js',
              {
                send : JSON.stringify([
                  btn.attr('model'),
                  btn.attr('name'),
                  Number(btn.attr('rid'))
                ])
              },
              function(data) {
                window.location.reload();
              }
          )
        }
        else {
          _this.form.oerpformview('update');
        }
      }
  );
};

FormView.prototype.initButtonAction = function() {
  var _this = this;
  var funcInit = function(ev) {
    ev.preventDefault();

    var btn = $(this);
    var param = {
      v: this,
      option: {
        width: 450
      },
      send : {
        model : btn.attr('model'),
        rid : btn.attr('rid'),
        act_id : btn.attr('name')
      }
    };

    UI.classes.OerpPromptEditor.createNew(param).init();
  };

  _this.form
      .find('input[type="submit"][button_type="action"]')
      .click(funcInit);
};

FormView.prototype.initButtonState = function() {
  var _this = this;
  var funcInit = function(ev) {
    ev.preventDefault();
    var btn = $(this);

    if (btn.attr('name') == 'end') {
      btn.parents('div.ui-dialog:eq(0)').
          find('a.ui-dialog-titlebar-close').click();
    }
    else {
      var dialog = btn.parents('div.oerp-formeditor:eq(0)');
      var value = dialog.data('getSend').call(dialog[0]);

      $.post(
          '?q=oerp/execute/js',
          {
            send : JSON.stringify(
                {
                  action : btn.attr('name'),
                  model: btn.attr('model'),
                  value : value,
                  datas : btn.attr('datas'),
                  wid : btn.attr('wid'),
                  rid : btn.attr('rid')
                }
            ),
            service : 'wizard'
          },
          function(data) {
            window.location.reload();
          }
      )
    }
  };

  _this.form
      .find('input[type="submit"][button_type="state"]')
      .click(funcInit);
};

FormView.prototype.initButtonWorkflow = function() {
  var _this = this;
  var funcInit = function(ev) {
    if (_this.isDialog()) {
      ev.preventDefault();
      var btn = $(this);
      btn.parents('.oerp-formeditor:eq(0)').dialog('disable');

      $.post(
          '?q=oerp/execute/js',
          {
            send : JSON.stringify([
              btn.attr('model'),
              btn.attr('name'),
              Number(btn.attr('rid'))
            ]),
            exec : 'exec_workflow'
          },
          function(data) {
            window.location.reload();
          }
      )
    }
    else {
      _this.form.oerpformview('update');
    }
  };

  _this.form
      .find('input[type="submit"][button_type="workflow"]')
      .click(funcInit);
};

FormView.prototype.prepareSubmit = function() {
  var _this = this;

  _this.form.find('table.oerp-treeview').each(function() {
    $tv = $(this);
    var pat = new RegExp('edit-view-(.*)-wrapper');
    var m = pat.exec($tv.parents('[id^="edit-"]:eq(0)').attr('id'));
    $tv.parents('div:eq(0)').children('hidden').remove();

    var recs = {};
    $tv.find('tr[oerp_rid]').each(function() {
      var val = $(this).data('vals');

      if (val !== undefined) {
        recs[$(this).attr('oerp_rid')] = val;
      }
    });

    var key = $tv.parents('div.oerp-view:eq(0)').attr('oerp_key');
    var send = escape(JSON.stringify(recs));
    $('#edit-' + key + '-json').attr('value', send);
  });

  _this.form.find('input[type="text"][rid]').each(
      function() {
        var $input = $(this);

        var pat = /(.*)\[name\]/;
        var name = pat.exec($input.attr('name'));

        $input.before('<input type="hidden" name="' + name[1]
            + '[rid]" value="' + $input.attr('rid') + '"/>');
      }
  );
};
/*
UI.components.OerpTabs = {
  createNew : function() {
    var obj = {};

    obj.init = function() {
      obj.$node.tabs();
    };
    return obj;
  }
};
*/