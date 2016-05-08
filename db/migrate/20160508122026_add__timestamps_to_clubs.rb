class AddTimestampsToClubs < ActiveRecord::Migration
  def change
    add_column :clubs, :created_at, :datetime
    add_column :clubs, :updated_at, :datetime
  end
end
