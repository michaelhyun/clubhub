class AddRatingToClubs < ActiveRecord::Migration
  def change
    add_column :clubs, :rating, :integer
  end
end
