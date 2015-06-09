(function($) {
  // TreeView
  var tree_methods = {
    init : function(params) {
      return this.each(function() {
        var v = new TreeView(this);
        v.init();
      });
    },
    update : function(params) {
      return this.each(function() {
        var v = new TreeView(this);
        v.prepareChecked();
      });
    }
  };

  $.fn.oerptreeview = function(method) {
    if (tree_methods[method]) {
      return tree_methods[method].apply(this, Array.prototype.slice.call(
          arguments, 1));
    } else if (typeof method === 'object' || !method) {
      return tree_methods.init.apply(this, arguments);
    }
  }
})(jQuery);

TreeView.prototype = new FormEvent();

function TreeView(selector) {
  this.view = (selector === undefined) ? undefined : $(selector);
  this.btn = undefined;
}

TreeView.prototype.attachButtonEvent = function(Ele)
{
  var $this = this;

  Ele.find('.oerp-btn-del').unbind('click').click(function() {
    $this.catchClk(this).delTr();
  });

  Ele.find('tbody .oerp-btn-chk, tbody .oerp-btn-select').unbind('click').click(function() {
    $this.catchClk(this).selTr();
  });

  Ele.find('.oerp-btn-edit').unbind('click').click(function() {
    $this.catchClk(this);

    if (!$this.getTr().hasClass('toDelete')) {
      $this.editTr($(this));
    }
  });

  Ele.find('.oerp-btn-add').unbind('click').click(
      function() {
        var $btn = $(this);
        $btn.parents('table:eq(0)').find(
            'tr[oerp_rid="0"] .oerp-btn-edit').click();
      }
  );

  var sig = Ele.attr('signature');
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

TreeView.prototype.init = function(){
  var $this = this;

  $this.attachButtonEvent($this.view);

  $this.view.prev('table.sticky-header:eq(0)').find('.oerp-btn-add').click(
      function() {
        $(this).parents('div.oerp-treeview:eq(0)').find(
            'table.oerp-treeview div.oerp-btn-add').click();
      });

  if ($this.view.hasClass('select-one')) {
    $this.view.find('tr[oerp_rid]').click(function() {
      var rid = $(this).attr('oerp_rid');
      $this.catchClk(this);
      $this.clearSel();
      $this.selTr(rid);
    });
  }
};

TreeView.prototype.prepareChecked = function() {
  var $this = this;
  var chks = [];

  $this.view.find('tr.selected').each(function() {
    chks.push($(this).attr('oerp_rid'));
  });

  var send = escape(JSON.stringify(chks));
  $('#edit-checked').attr('value', send);
};

TreeView.prototype.getTr = function(rid) {
  var $this = this;
  var tr;

  if (rid === undefined) {
    tr = $this.btn.parents('tr[oerp_rid]:eq(0)')
  } else {
    tr = $this.view.find('tr[oerp_rid="' + rid + '"]');
    this.btn = tr.children('td:eq(0)');
  }

  return (tr.length > 0) ? tr : undefined;
};

TreeView.prototype.updateTr = function(rid, vals) {
  var $this = this;
  var tr = $this.getTr(rid);
  tr.data('val', vals);

  tr.children('td[oerp_field]').each(
      function() {
        var td = $(this);
        var val = vals['#value']['view[' + td.attr('oerp_field') + ']'];

        if (val == undefined) {
          if (td.children('span.oerp-changed').length == 0) {
            td.html(
                '<span class="oerp-changed">' + td.html() + '</span>');
          }
        } else {
          if (val['#text']) {
            td.html(val['#text']);
          } else {
            td.html(val['#value']);
          }
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

  $this.attachButtonEvent(tr);
};

TreeView.prototype.addTr = function(val, rid) {
  var $this = this;

  rid = (rid === undefined) ? '0-'
      + $this.getTree().find('tr.oerp-new').length : rid;

  var newTr = $this.getTpl().clone(true);
  newTr
      .attr('oerp_rid', rid)
      .removeClass('oerp-tpl')
      .removeClass('hide')
      .addClass('oerp-new');

  $this.getTree().removeClass('empty').find('tbody:eq(0)').append(newTr);
  $this.updateTr(rid, val);
};

TreeView.prototype.modifyTr = function(send) {
  var $this = this;

  if ($this.getRId() == 0) {
    $this.addTr(send);
  } else {
    $this.updateTr($this.getRId(), send);
  }
};

TreeView.prototype.getTpl = function() {
  var $this = this;
  var tr = $this.getTree().find('tr.oerp-tpl');
  return (tr.length > 0) ? tr : false;
};

TreeView.prototype.getRId = function() {
  var $this = this;
  return $this.getTr().attr('oerp_rid');
};

TreeView.prototype.getVId = function(level) {
  var $this = this;
  level = (level === undefined) ? 0 : level;

  var vid = $this.btn.parents('[oerp_vid]:eq(' + level + ')').attr('oerp_vid');
  return (vid == undefined) ? '' : vid;
};

TreeView.prototype.getCacheId = function() {
  var $this = this;
  return $this.getView().attr('cache_id');
};

TreeView.prototype.getView = function() {
  var $this = this;
  return $this.view;
};

TreeView.prototype.getTree = function() {
  var $this = this;
  var tree = $this.getView().find('table.oerp-treeview:eq(0)');
  return (tree.length > 0) ? tree : false;
};

TreeView.prototype.getKey = function() {
  var $this = this;
  return $this.getView().attr('oerp_key');
};

TreeView.prototype.getModel = function(level) {
  var $this = this;
  level = (level === undefined) ? 0 : level;

  var mod = $this.btn
      .parents('[oerp_model]:eq(' + level + ')')
      .attr('oerp_model');

  return (mod === undefined) ? '' : mod;
};

TreeView.prototype.getName = function() {
  var $this = this;
  return $this.getView().attr('oerp_name');
};

TreeView.prototype.getValue = function() {
  var $this = this;
  var val = $('#edit-' + $this.getKey() + '---json-' + $this.getRId());
  return JSON.parse(val.attr('value'));
};

TreeView.prototype.delTr = function(rid) {
  var $this = this;
  var tr = $this.getTr(rid);

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

TreeView.prototype.clearSel = function() {
  var $this = this;

  $this.view.find('tr.selected').removeClass('selected');
  $this.view.find('td.oerp-op div.oerp-button')
      .addClass('ui-state-default')
      .removeClass('ui-state-highlight');
};

TreeView.prototype.selTr = function(rid) {
  var $this = this;
  var tr = $this.getTr(rid);
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

TreeView.prototype.editTr = function() {
  var $this = this;
  new PromptEditor({
    v : $this,
    vals : $this.getTr().data('val'),
    option : {
      buttons : {
        'Update' : function() {
          var $editor = $(this);
          var v = $editor.data('v');
          var resp = $editor.data('getSend').call(this);

          if(resp){
            v.modifyTr(resp);
            $editor.remove();
          }
          else{
            alert('Some required fields are missing. Please check again.');
          }
        }
      }
    },
    send : {
      par_model : $this.getModel(1),
      par_vid : $this.getVId(1),
      model : $this.getModel(),
      vid : $this.getVId(0),
      name : $this.getName(),
      rid : $this.getRId(),
      cache_id : $this.getCacheId(),
      type : 'form'
    }
  });
};