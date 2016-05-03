class CreateCategories < ActiveRecord::Migration
  def change
    create_table :categories do |t|
      t.string :genre

      t.timestamps
    end
  end
end
