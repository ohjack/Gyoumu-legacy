/*
UI.components.OerpTreeWidget = {
  createNew: function() {
    var obj = {};

    obj.catchClk = function(clicked) {
      obj.$clicked = $(clicked);
      return obj;
    };

    obj.getTr = function() {
      if (obj.$clicked.get(0).tagName === 'TR') {
        return obj.$clicked;
      }
      else {
        return obj.$clicked.parents('tr:eq(0)');
      }
    };

    obj.delTr = function() {
      var tr = obj.getTr();

      if (tr) {
        var val = tr.data('val');

        if (tr.hasClass('toDelete')) {
          delete val['#delete'];
          (val['#value'] === undefined) ? tr.removeData('val') : tr.data('val', val);
        }
        else {
          if (val == undefined) {
            tr.data('val', {
              '#delete' : true
            });
          } else {
            val['#delete'] = true;
            tr.data('val', val);
          }
        }

        tr.toggleClass('toDelete');
        tr.nextAll('tr').each(function(){
          var next_tr = $(this);

          if(next_tr.attr('merged')){
            next_tr.toggleClass('toDelete');
          }
          else{
            return false;
          }
        })
      }
    };

    obj.selTr = function() {
      var tr = obj.getTr();
      var op = tr.find('td.oerp-op div.oerp-button');

      if (tr) {
        tr.toggleClass('selected');
        tr = tr.next('tr');

        while(tr.length){
          if(tr.attr('merged')){
            tr.toggleClass('selected');
            tr = tr.next('tr');
          }
          else{
            break;
          }
        }
      }
    };

    obj.getModel = function(level) {
      level = (level === undefined) ? 0 : level;

      var mod = obj.$clicked
          .parents('[oerp_model]:eq(' + level + ')')
          .attr('oerp_model');

      return (mod === undefined) ? '' : mod;
    };

    obj.getVId = function(level) {
      level = (level === undefined) ? 0 : level;

      var vid = obj.$clicked.parents('[oerp_vid]:eq(' + level + ')').attr('oerp_vid');
      return (vid == undefined) ? '' : vid;
    };

    obj.getName = function() {
      return obj.getView().attr('oerp_name');
    };

    obj.getView = function() {
      return obj.$node.find('div.oerp-view');
    };

    obj.getRId = function() {
      return obj.getTr().attr('oerp_rid');
    };

    obj.getCacheId = function() {
      return obj.getView().attr('cache_id');
    };

    obj.getTree = function() {
      var tree = obj.getView().find('table.oerp-treeview:eq(0)');
      return (tree.length > 0) ? tree : false;
    };

    obj.getTpl = function() {
      var tr = obj.getTree().find('tr.oerp-tpl');
      return (tr.length > 0) ? tr : false;
    };

    obj.addTr = function(val, rid) {
      rid = (rid === undefined) ? '0-'
          + obj.getTree().find('tr.oerp-new').length : rid;

      var newTr = obj.getTpl().clone(true);
      newTr
          .attr('oerp_rid', rid)
          .removeClass('oerp-tpl')
          .removeClass('hide')
          .addClass('oerp-new');

      obj.getTree().removeClass('empty').find('tbody:eq(0)').append(newTr);
      obj.updateTr(rid, val);
    };

    obj.updateTd = function(td, val) {
      if (val['#text']) {
        td.html(val['#text']);
      } else {
        td.html(val['#value']);
      }
    };

    obj.updateTr = function(rid, vals) {
      var tr = obj.$node.find('tr[oerp_rid="' + rid + '"]');
      tr.data('val', vals);

      tr.children('td[oerp_field]').each(function () {
        var td = $(this);
        var val = vals['#value']['view[' + td.attr('oerp_field') + ']'];

        if (val == undefined) {
          if (td.attr('gu_alt_name')) {
            val = vals['#value']['view[' + td.attr('gu_alt_name') + ']'];
            obj.updateTd(td, val);
          }
          else if (td.children('span.oerp-changed').length == 0) {
            td.html(
                '<span class="oerp-changed">' + td.html() + '</span>');
          }
        } else {
          obj.updateTd(td, val);
        }
      });

      tr.addClass('toChange');
      tr.nextAll('tr').each(function(){
        var next_tr = $(this);
        if(!next_tr.attr('merged')){
          return false;
        }
        else{
          next_tr.addClass('toChange');
        }
      });

      obj.initBtns(tr);
    };

    obj.modifyTr = function(send) {
      if (obj.getRId() == 0) {
        obj.addTr(send);
      } else {
        obj.updateTr(obj.getRId(), send);
      }
    };

    obj.editTr = function() {
      UI.classes.OerpOne2manyPromptEditor.createNew(obj).init();
    };

    obj.initBtns = function(ele) {
      ele.find('.oerp-btn-del').unbind('click').click(function() {
        obj.catchClk(this).delTr();
      });

      ele.find('.oerp-btn-chk, .oerp-btn-select').unbind('click').click(function() {
        obj.catchClk(this).selTr();
      });

      ele.find('.oerp-btn-edit').unbind('click').click(function() {
        if (!obj.catchClk(this).getTr().hasClass('toDelete')) {
          obj.editTr();
        }
      });

      ele.find('.oerp-btn-add').unbind('click').click(function () {
        obj.catchClk(this)
            .getTr()
            .parents('table:eq(0)')
            .find('tr[oerp_rid="0"] .oerp-btn-edit').click();
      });

      var tree = obj.getTree();
      tree.prev('table.sticky-header:eq(0)')
          .find('.oerp-btn-add')
          .click(function () {
            $(this).parents('div.oerp-treeview:eq(0)').find(
                'table.oerp-treeview div.oerp-btn-add').click();
          });

      if (tree.hasClass('select-one')) {
        tree.find('tr[oerp_rid]').click(function() {
          obj.catchClk(this);
          obj.clearSel();
          obj.selTr();
        });
      }

      var sig = tree.attr('signature');
      $('thead .oerp-btn-select').unbind('click').click(function(){
        var $chk = $(this);
        var selected = $chk.hasClass('selected');
        $chk.toggleClass('selected');

        var $trs = $('tbody div.oerp-btn-select[signature=' + sig + ']:eq(0)')
            .parents('table:eq(0)')
            .find('tr')
            .not('.oerp-tpl');

        if(selected){
          $trs.removeClass('selected');
        }
        else{
          $trs.filter('.sep-after').removeClass('selected');
          $trs.not('.sep-after').addClass('selected');
        }
      });
    };

    obj.clearSel = function() {
      var view = obj.getView();
      view.find('tr.selected').removeClass('selected');
      view.find('td.oerp-op div.oerp-button')
          .addClass('ui-state-default')
          .removeClass('ui-state-highlight');
    };

    obj.init = function() {
      obj.initBtns(obj.$node);
    };
    return obj;
  }
};
*/

/*
TreeView.prototype.prepareChecked = function() {
  var $this = this;
  var chks = [];

  $this.view.find('tr.selected').each(function() {
    chks.push($(this).attr('oerp_rid'));
  });

  var send = escape(JSON.stringify(chks));
  $('#edit-checked').attr('value', send);
};

TreeView.prototype.getKey = function() {
  var $this = this;
  return $this.getView().attr('oerp_key');
};

TreeView.prototype.getValue = function() {
  var $this = this;
  var val = $('#edit-' + $this.getKey() + '---json-' + $this.getRId());
  return JSON.parse(val.attr('value'));
};
*/