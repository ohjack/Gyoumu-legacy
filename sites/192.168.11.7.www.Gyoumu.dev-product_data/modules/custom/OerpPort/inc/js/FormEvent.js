FormEvent.prototype.catchClk = function(clicked) {
  this.btn = $(clicked);
  this.view = this.btn.parents('div.oerp-view:eq(0)');
  return this;
}

function FormEvent() {}
