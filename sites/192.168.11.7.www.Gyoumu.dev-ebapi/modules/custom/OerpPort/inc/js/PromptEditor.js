/*
UI.components.OerpJsEditor = {
  createNew: function(){
    var obj = {};

    obj.getSend = function() {
      var _this = this;
      var $form = $(_this.$node).find('form:eq(0)');
      var send = {
        '#value' : {}
      };

      // sync treeview records
      $form.find('div.oerp-treeview').each(
          function() {
            var $treeview = $(this);
            var send_inner = {};

            $treeview.find('tr[oerp_rid]').each(function() {
              var $this = $(this);

              if ($this.data('val')) {
                send_inner[$this.attr('oerp_rid')] = $this.data('val');
              }
            });

            var fld_name = $treeview.children('input[type="hidden"]:eq(0)').attr(
                'name').replace('[json]', '');

            send['#value'][fld_name] = send_inner;
          }
      );

      var post = $form.serializeArray();
      $form.find('input:checkbox').each(function() {
        var $checkbox = $(this);

        if ($checkbox.attr('checked') == false) {
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

    obj.save = function(func_aftersave) {
      var _this = this;
      var view = _this.$node.find('div.oerp-view:eq(0)');
      var model = view.attr('oerp_model');
      var rid = Number(view.attr('oerp_rid'));

      if (rid == 0) {
        var send = [model, 'create', _this.getSend()];
      } else {
        var send = [model, 'write', [ rid ], _this.getSend()];
      }

      // update
      $.post(
          '?q=oerp/execute/js',
          { send:JSON.stringify(send) },
          function(rs) { func_aftersave(rs) }
      );
    };

    obj.init = function(){
      var _this = this;
      $.post('?q=oerp/view/editor/js', _this.$node.data('send'), function(json){
        _this.$node.html(decodeURIComponent(eval(json)));
        UI.awake(_this.$node);
      });
    };
    return obj;
  },
  createEle: function(send, dvals){
    var ele = $('<div class="awakable" data-awake="OerpJsEditor"/>');
    ele.data('send', send).data('dvals', dvals);
    ele.awake = function() {
      UI.awake(ele.parent());
    };
    return ele;
  }
};

UI.components.OerpPrompt = boo.Base.derive({
  getName: function() { return 'OerpPrompt'; },
  createNew: function() {
    var obj = boo.Base.derive({});

    obj.close = function () {
      var _this = this;
      _this.$node.dialog('destroy').remove();
    };

    obj.getOption = function() {
      var _this = this;

      return {
        close: function() { _this.close(); },
        width: 974,
        modal: true,
        autoOpen: false
      }
    };

    obj.init = function() {
      var _this = this;
      _this.$node.dialog(_this.getOption());

      // auto-adjust vertical position
      var wrapper = _this.$node.parents('div.ui-dialog:eq(0)');
      var bottom = wrapper.height() + wrapper.offset().top + 10;
      var win_height = $(window).height();

      _this.$node.dialog('option', 'position', 'center');
      height = wrapper.height() + wrapper.offset().top + 10;

      if ($('div.ui-widget-overlay').height() < height) {
        $('div.ui-widget-overlay').height(height);
      }

      _this.$node.html(_this.$node.data('$inner'));
      UI.awake(_this.$node);
      _this.$node.dialog('open');
    };

    return obj;
  },
  createEle: function(UIParent, $inner) {
    var pool = $('div#dialog-pool');

    if (!pool.length) {
      pool = $('<div id="dialog-pool"></div>').appendTo('body');
    }

    var src = '<div class="awakable connected" data-awake="' + this.getName() + '" data-belongsto="' + UIParent.$node.attr('id') + '"/>';
    var ele = $(src).appendTo(pool);

    ele.data('UIParent', UIParent).data('$inner', $inner);
    ele.awake = function() {
      return UI.awake(ele.parent());
    };
    return ele;
  }
});

UI.components.OerpJsEditorPrompt = UI.components.OerpPrompt.derive({
  getName: function() { return 'OerpJsEditorPrompt'; },
  createEle: function(UIParent, send) {
    var $inner = UI.components.OerpJsEditor.createEle(send);
    console.dir(this);
    console.dir(this.__proto__);
//    return this.__proto__.createEle.call(this, UIParent, $inner);
  }
});

UI.components.OerpPromptForm = UI.components.OerpJsEditorPrompt.derive({
  getName: function() { return 'OerpPromptForm'; },
  createNew: function() {
    var obj = this.__proto__.createNew.call(this).derive({});

    obj.save = function() {
      var _this = this;
      var Inner = UI.getCompo(_this.$node.data('$inner'));

      Inner.save(function(rs) {
        var rid = eval(rs);
        var UIParent = _this.$node.data('UIParent');
        UIParent.changeRId(rid);
        _this.close();
      });
    };

    obj.getOption = function() {
      var _this = this;
      var opts = _this.__proto__.getOption.call(_this);

      opts['buttons'] = {
        'Save': function() { _this.save(); }
      };
      return opts;
    };
    return obj;
  }

  createEle: function(UIParent, send) {
    var $inner = UI.components.OerpJsEditor.createEle(send);
    return this.__proto__.createEle.call(this, UIParent, $inner);
  }
});

UI.components.OerpPromptMany2oneSelector = UI.components.OerpPrompt.derive({
  getName: function() { return 'OerpPromptMany2oneSelector'; },
  createNew: function() {
    var obj = this.__proto__.createNew.call(this).derive({});

    obj.save = function() {
      var _this = this;

      Inner.save(function(rs) {
        var rid = eval(rs);
        var UIParent = _this.$node.data('UIParent');
        UIParent.changeRId(rid);
        _this.close();
      });
    };

    obj.select = function() {
      var _this = this;
      var UIParent = _this.$node.data('UIParent');
      var $inner = _this.$node.data('$inner');
      var rid = $inner.find('tr.selected').attr('oerp_rid');

      if (rid !== undefined) {
        UIParent.changeRId(rid);
      }
      _this.close();
    };

    obj.formview = function() {
      var _this = this;
      var UIParent = _this.data('UIParent');

      _this.close();

      UI.components.OerpPromptForm.createEle(
          UIParent, UIParent.getFormSendData(), true).awake();
    };

    obj.getOption = function() {
      var _this = this;
      var opts = _this.__proto__.getOption.call(_this);

      opts['maxHeight'] = 400;
      opts['buttons'] = {
        'OK' : function() { _this.select(); },
        'New' : function() { _this.formview(); }
      };
      return opts;
    };
    return obj;
  },
  createEle: function(UIParent, send) {
    var $inner = UI.components.OerpJsEditor.createEle(send);
    return this.__proto__.createEle.call(this, UIParent, $inner);
  }
});

UI.classes.OerpOne2manyPromptEditor = {
  createNew: function(widget) {
    var param = {
      send: {
        par_model : widget.getModel(1),
        par_vid : widget.getVId(1),
        model : widget.getModel(0),
        vid : widget.getVId(0),
        name : widget.getName(),
        rid : widget.getRId(),
        cache_id : widget.getCacheId(),
        type : 'form'
      }
    };

    var obj = UI.classes.OerpPromptEditor.createNew(param);
    obj.widget = widget;

    obj.update = function() {
      var resp = obj.getSend();

      if(resp){
        obj.widget.modifyTr(resp);
        obj.dialog.remove();
      }
      else{
        alert('Some required fields are missing. Please check again.');
      }
    };

    obj.getBtnConf = function() {
      return {'Update': function(){ obj.update(); }};
    };
    return obj;
  }
};

UI.classes.OerpSelectOnePromptEditor = {
  createNew: function(widget) {
    var param = {
      send : {
        model : widget.getModel(),
        name : widget.getName(),
        type : 'tree',
        domain : widget.getDomain()
      }
    };

    var obj = UI.classes.OerpPromptEditor.createNew(param);
    obj.widget = widget;

    obj.select = function() {
      var rid = obj.dialog.find('tr.selected').attr('oerp_rid');

      if (rid !== undefined) {
        widget.changeRId(rid);
      }
      obj.dialog.remove();
    };

    obj.formview = function() {
      UI.classes.OerpMany2onePromptEditor.createNew(widget, true).init();
      obj.dialog.remove();
    };

    obj.option['maxHeight'] = 400;

    obj.getBtnConf = function() {
      return {
        'OK' : function() { obj.select(); },
        'New' : function() { obj.formview(); }
      };
    };
    return obj;
  }
};
*/